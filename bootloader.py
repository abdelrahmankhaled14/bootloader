#!/usr/bin/env python3
"""
Safe STM32 Bootloader Client with ELF Parser
- Writes flash in 128-byte aligned chunks
- Starts from beginning of first loadable section
- Uses entry point for info only
"""

import serial
import time
import struct
import sys
from pathlib import Path

# =============================
# Protocol Constants
# =============================
SOF = 0x55
CMD_WRITE = 0x01
CMD_VERSION = 0x03
CMD_DONE = 0x04

ACK = 0xAA
NACK = 0xFF

ACK_TIMEOUT = 10.0

BOOTLOADER_MIN_ADDRESS = 0x08001000
BOOTLOADER_MAX_ADDRESS = 0x0803FFFF
CHUNK_SIZE = 128  # Safe aligned chunk size for STM32

# =============================
# Logging
# =============================
def log(level, msg):
    colors = {
        'SEND': '\033[94m',
        'RECV': '\033[92m',
        'OK': '\033[92m',
        'ERROR': '\033[91m',
        'WARN': '\033[93m',
        'INFO': '\033[94m',
        'RESET': '\033[0m'
    }
    color = colors.get(level, colors['INFO'])
    timestamp = time.strftime('%H:%M:%S')
    print(f"{color}[{timestamp}] [{level:6s}]{colors['RESET']} {msg}")

# =============================
# ELF Parser
# =============================
class ELFParser:
    def __init__(self, elf_file):
        self.elf_file = elf_file
        self.data = None
        self.sections = []
        self.entry_point = None
        self._parse()

    def _parse(self):
        with open(self.elf_file, 'rb') as f:
            self.data = f.read()

        if self.data[:4] != b'\x7fELF':
            raise ValueError("Not a valid ELF file")

        # Entry point
        self.entry_point = struct.unpack('<I', self.data[24:28])[0]
        log('INFO', f"ELF entry point: 0x{self.entry_point:08X}")

        # Parse 32-bit program headers
        ei_class = self.data[4]
        if ei_class != 1:
            raise ValueError("Only 32-bit ELF supported")

        e_phoff = struct.unpack('<I', self.data[28:32])[0]
        e_phentsize = struct.unpack('<H', self.data[42:44])[0]
        e_phnum = struct.unpack('<H', self.data[44:46])[0]

        log('INFO', f"Found {e_phnum} program headers")

        for i in range(e_phnum):
            ph_offset = e_phoff + i * e_phentsize
            p_type = struct.unpack('<I', self.data[ph_offset:ph_offset+4])[0]
            p_offset = struct.unpack('<I', self.data[ph_offset+4:ph_offset+8])[0]
            p_paddr = struct.unpack('<I', self.data[ph_offset+12:ph_offset+16])[0]
            p_filesz = struct.unpack('<I', self.data[ph_offset+16:ph_offset+20])[0]

            if p_type == 1 and p_filesz > 0:  # PT_LOAD
                section_data = self.data[p_offset:p_offset+p_filesz]
                log('INFO', f"  Section {i}: addr=0x{p_paddr:08X}, size={p_filesz} bytes")
                self.sections.append({
                    'address': p_paddr,
                    'size': p_filesz,
                    'data': section_data
                })

    def get_sections(self):
        return self.sections

    def get_entry_point(self):
        return self.entry_point

# =============================
# Serial Protocol
# =============================
def wait_for_ack(ser, step_name, timeout=ACK_TIMEOUT):
    start = time.time()
    while time.time() - start < timeout:
        if ser.in_waiting > 0:
            response = ser.read(1)[0]
            if response == ACK:
                log('OK', f"{step_name} -> ACK ✓")
                return True
            elif response == NACK:
                log('ERROR', f"{step_name} -> NACK ✗")
                return False
            else:
                log('WARN', f"{step_name} -> Unexpected: 0x{response:02X}")
        time.sleep(0.001)
    log('ERROR', f"{step_name} -> TIMEOUT")
    raise RuntimeError(f"ACK timeout on {step_name}")

def send_byte(ser, byte_val, description=""):
    log('SEND', f"{description}: 0x{byte_val:02X}")
    ser.write(bytes([byte_val]))
    ser.flush()
    if not wait_for_ack(ser, description):
        raise RuntimeError(f"Failed at {description}")

def send_data(ser, data, description=""):
    log('SEND', f"{description}: {data.hex().upper()} ({len(data)} bytes)")
    ser.write(data)
    ser.flush()
    if not wait_for_ack(ser, description):
        raise RuntimeError(f"Failed at {description}")

def reset_rx_buffer(ser):
    ser.reset_input_buffer()
    time.sleep(0.05)

# =============================
# Commands
# =============================
def cmd_write(ser, address, data):
    if not (BOOTLOADER_MIN_ADDRESS <= address < BOOTLOADER_MAX_ADDRESS):
        raise ValueError(f"Address 0x{address:08X} out of range")
    if len(data) == 0 or len(data) > CHUNK_SIZE:
        raise ValueError(f"Data length {len(data)} invalid")

    try:
        reset_rx_buffer(ser)
        log('INFO', f"WRITE: 0x{address:08X} ({len(data)} bytes)")
        send_byte(ser, SOF, "SOF")
        send_byte(ser, CMD_WRITE, "CMD_WRITE")
        send_byte(ser, len(data), "LENGTH")
        send_data(ser, struct.pack(">I", address), "ADDRESS")
        send_data(ser, data, "DATA")
        if not wait_for_ack(ser, "WRITE_COMPLETE"):
            raise RuntimeError("Write command failed")
        log('OK', "WRITE successful ✓")
        return True
    except Exception as e:
        log('ERROR', f"WRITE failed: {str(e)}")
        return False

def cmd_version(ser):
    try:
        reset_rx_buffer(ser)
        log('INFO', "Getting version...")
        send_byte(ser, SOF, "SOF")
        send_byte(ser, CMD_VERSION, "CMD_VERSION")
        start = time.time()
        while time.time() - start < ACK_TIMEOUT:
            if ser.in_waiting > 0:
                version = ser.read(1)[0]
                log('OK', f"Bootloader Version: 0x{version:02X}")
                return version
            time.sleep(0.001)
        raise RuntimeError("Timeout waiting for version")
    except Exception as e:
        log('ERROR', f"VERSION failed: {str(e)}")
        return None

def cmd_done(ser):
    try:
        reset_rx_buffer(ser)
        log('INFO', "Sending DONE command...")
        send_byte(ser, SOF, "SOF")
        send_byte(ser, CMD_DONE, "CMD_DONE")
        log('OK', "DONE sent - MCU jumping to application")
        return True
    except Exception as e:
        log('ERROR', f"DONE failed: {str(e)}")
        return False

# =============================
# Main
# =============================
def main():
    if len(sys.argv) < 2:
        print("Usage: python3 bootloader.py <elf_file> [port] [baudrate]")
        return False

    elf_file = sys.argv[1]
    port = sys.argv[2] if len(sys.argv) > 2 else "/dev/ttyUSB0"
    baudrate = int(sys.argv[3]) if len(sys.argv) > 3 else 115200

    if not Path(elf_file).exists():
        log('ERROR', f"File not found: {elf_file}")
        return False

    log('INFO', f"ELF Bootloader Client")
    log('INFO', f"File: {elf_file}")
    log('INFO', f"Port: {port}, Baudrate: {baudrate}")

    try:
        parser = ELFParser(elf_file)
        sections = parser.get_sections()
        entry_point = parser.get_entry_point()
        if not sections:
            log('ERROR', "No loadable sections found in ELF")
            return False

        # Sort sections by address
        sections.sort(key=lambda s: s['address'])
        ser = serial.Serial(port, baudrate, timeout=0.5)
        time.sleep(0.5)
        log('OK', "Serial port opened")

        # Get bootloader version
        version = cmd_version(ser)
        if version is None:
            log('WARN', "Could not get bootloader version (continuing anyway)")
        time.sleep(0.2)

        # Program all sections from beginning
        total_written = 0
        for i, section in enumerate(sections):
            address = section['address']
            data = section['data']
            size = section['size']

            # Ensure 4-byte alignment
            align_offset = address % 4
            if align_offset != 0:
                pad = 4 - align_offset
                log('WARN', f"Section address not aligned, adding {pad} padding bytes")
                address -= align_offset
                data = b'\x00'*pad + data
                size = len(data)

            log('INFO', f"\n=== Section {i+1}/{len(sections)} ===")
            log('INFO', f"Address: 0x{address:08X}, Size: {size} bytes")

            # Send in 128-byte chunks
            for offset in range(0, size, CHUNK_SIZE):
                chunk = data[offset:offset+CHUNK_SIZE]
                chunk_addr = address + offset
                if not cmd_write(ser, chunk_addr, chunk):
                    log('ERROR', "Programming failed!")
                    ser.close()
                    return False

                total_written += len(chunk)
                progress = (total_written / sum(s['size'] for s in sections)) * 100
                log('INFO', f"Progress: {progress:.1f}% ({total_written} bytes)")
                time.sleep(0.05)

        log('OK', f"\n✓ All sections programmed ({total_written} bytes)")
        time.sleep(0.5)
        if cmd_done(ser):
            log('OK', "✓ Application should now be running!")

        ser.close()
        return True

    except Exception as e:
        log('ERROR', f"Fatal error: {str(e)}")
        import traceback
        traceback.print_exc()
        return False


if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
