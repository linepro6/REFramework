from translate.storage import po
from io import BytesIO
import csv

FNV1_32_INIT = 0x811c9dc5
FNV_32_PRIME = 0x01000193

def fnv1a_32(data, hval=FNV1_32_INIT):
    for byte in data:
        hval ^= byte
        hval = (hval * FNV_32_PRIME) & 0xffffffff
    return hval

def po2ymohpp(lang, includefuzzy=False, encoding='utf-8', byteorder='little'):
    inputstore = po.pofile(open(f"{lang}.po", 'rb'))

    units = {}
    for unit in inputstore.units:
        if unit.istranslated() or (unit.isfuzzy() and includefuzzy and unit.target):
            source = unit.source
            context = unit.getcontext()
            print(source, "-----", unit.target)
            if context:
                source = context + '\004' + source
            hash = fnv1a_32(source.encode(encoding))
            units[hash] = unit.target.encode(encoding) + bytes(2)
    
    with open(f"{lang}_mods_text.csv", "r", encoding="utf-8-sig") as csvf:
        reader = csv.reader(csvf)
        for line in reader:
            if len(line) != 2:
                continue
            source, target = line
            if source and target:
                source = source.replace("\\", "")
                print(source, "-----", target)
                hash = fnv1a_32(source.encode(encoding))
                units[hash] = target.encode(encoding) + bytes(2)
        csvf.close()


    f = BytesIO()
    f.write(len(units).to_bytes(2, byteorder)) # len

    offset = 2 + len(units) * (4 + 2)
    for hash, data in units.items():
        f.write(hash.to_bytes(4, byteorder))
        f.write(offset.to_bytes(2, byteorder))
        offset += len(data)

    for data in units.values():
        f.write(data)
    
    with open(f"../src/I18n/i18n_ymo_{lang}.hpp", "w") as wf:
        wf.write("static const unsigned char i18n_ymo_%s[] = { " % lang)
        wf.write(", ".join([hex(b) for b in f.getvalue()]))
        wf.write(" };")
        wf.close()
    
    # with open("out.ymo", "wb") as wf2:
    #     wf2.write(f.getvalue())
    #     wf2.close()

if __name__ == '__main__':
    po2ymohpp("zh_CN")
