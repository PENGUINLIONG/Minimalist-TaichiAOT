from collections import defaultdict
import subprocess
import os
from typing import Dict, List

IMPL_DIR = "build-taichi-ios-arm64/taichi.build/Debug-iphoneos/taichi_c_api.build/Objects-normal/arm64"
SEARCH_ROOT = "build-taichi-ios-arm64"

class Symbol:
    def __init__(self):
        self.defined_in = []
        self.depended_on_by = []

def dump_symbols(o: str) -> Dict[str, str]:
    symbols = str(subprocess.check_output(["objdump", "--demangle", "--syms", o]), encoding="utf8").splitlines()

    while not symbols[0].startswith("SYMBOL TABLE:"):
        symbols = symbols[1:]
        continue
    symbols = symbols[1:]

    offset = len("0000000000000000         ")
    out = defaultdict(list)
    for x in symbols:
        x = x[offset:]
        k, v = x.split(' ', 1)
        out[k] += [v]
    return out

# symbol name -> obj file paths
SYMBOL_DEFS = defaultdict(list)
# obj file path -> undefined symbol names
UNDEFINED_SYMBOLS = defaultdict(list)
for (base_dir, _, file_names) in os.walk(SEARCH_ROOT, topdown=True):
    for file_name in file_names:
        o = f"{base_dir}/{file_name}"
        if o.endswith(".o"):
            for k, syms in dump_symbols(o).items():
                if k == "*UND*":
                    # Undefined symbols.
                    for x in syms:
                        UNDEFINED_SYMBOLS[o] += [x]
                        SYMBOL_DEFS[x] += []
                else:
                    # Defined symbols.
                    for x in syms:
                        SYMBOL_DEFS[x] += [o]

if True:
    print("all taichi symbols:")
    for symbol_name, sym in SYMBOL_DEFS.items():
        print(f"  [[ {symbol_name} ]]")
        for x in sym:
            print(f"    {x}")

for sym, defs in SYMBOL_DEFS.items():
    defs = set(defs)
    if len(defs) == 0:
        print(f"`{sym}` is never defined")
        #assert False
    if len(defs) > 1:
        print(f"`{sym}` defined in multiple files: {defs}")
        #assert False
print()

ROOT_OBJS = []
for x in os.listdir(IMPL_DIR):
    if not x.endswith(".o"):
        continue
    ROOT_OBJS += [f"{IMPL_DIR}/{x}"]

print("found the following c-api root objects:")
for x in ROOT_OBJS:
    print(f"  {IMPL_DIR}/{x}")
print()

C_API_DEPS = [x for x in ROOT_OBJS]
for o in ROOT_OBJS:
    syms = dump_symbols(o)
    for x in syms["*UND*"]:
        C_API_DEPS += SYMBOL_DEFS[x]

# Cascade dependency and fetch all indirect object dependencies.
LAST_DEPS = str(sorted(C_API_DEPS))
CUR_DEPS = ""
DEPS = sorted(set(C_API_DEPS))
while LAST_DEPS != CUR_DEPS:
    print("cascading...")
    LAST_DEPS = CUR_DEPS

    dep_symbols = []
    for x in DEPS:
        dep_symbols += UNDEFINED_SYMBOLS[x]

    next_deps = DEPS
    for x in set(dep_symbols):
        next_deps += SYMBOL_DEFS[x]

    DEPS = sorted(set(next_deps))
    CUR_DEPS = str(DEPS)

print("dependent objects:")
for dep in set(DEPS):
    print(f"  {dep}")
print()
