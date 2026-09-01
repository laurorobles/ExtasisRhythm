import random
import csv

SALT_1 = 0xA784F19E3B5D28C6
SALT_2 = 0x6E4C90B21F87D35A

def generate_key():
    val1 = random.randint(1, 0xFFFF)
    
    seed = (val1 << 32) | val1
    
    calc1 = (seed ^ SALT_1) & 0xFFFFFFFFFFFFFFFF
    calc1 = (calc1 * 0x45D9F3B) & 0xFFFFFFFFFFFFFFFF
    expected2 = (calc1 >> 16) & 0xFFFF
    
    seed_shift = ((seed << 13) & 0xFFFFFFFFFFFFFFFF) | (seed >> 19)
    calc2 = (seed_shift ^ SALT_2) & 0xFFFFFFFFFFFFFFFF
    calc2 = (calc2 * 0x27D4EB2D) & 0xFFFFFFFFFFFFFFFF
    expected3 = (calc2 >> 16) & 0xFFFF
    
    expected4 = ((val1 ^ expected2 ^ expected3 ^ 0xBEEF) * 0x119DE1) & 0xFFFF
    
    return f"EXTR-{val1:04X}-{expected2:04X}-{expected3:04X}-{expected4:04X}"

keys = set()
while len(keys) < 5000:
    keys.add(generate_key())

with open('extasis_rhythm_licenses.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    for k in keys:
        writer.writerow([k])

print("Generated 5000 keys in extasis_rhythm_licenses.csv")
