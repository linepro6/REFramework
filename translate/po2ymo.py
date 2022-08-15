from translate.storage import po
from io import BytesIO

FNV1_32_INIT = 0x811c9dc5
FNV_32_PRIME = 0x01000193

def fnv1a_32(data, hval=FNV1_32_INIT):
    for byte in data:
        hval ^= byte
        hval = (hval * FNV_32_PRIME) & 0xffffffff
    return hval

def po2ymohpp(pofilename, includefuzzy=False, encoding='utf-8', byteorder='little'):
    inputstore = po.pofile(open(pofilename, 'rb'))

    units = {}
    for unit in inputstore.units:
        if unit.istranslated() or (unit.isfuzzy() and includefuzzy and unit.target):
            source = unit.source
            context = unit.getcontext()
            if context:
                source = context + '\004' + source
            hash = fnv1a_32(source.encode(encoding))
            units[hash] = unit.target.encode(encoding) + bytes(2)

    f = BytesIO()
    f.write(len(units).to_bytes(2, byteorder)) # len

    offset = 2 + len(units) * (4 + 2)
    for hash, data in units.items():
        f.write(hash.to_bytes(4, byteorder))
        f.write(offset.to_bytes(2, byteorder))
        offset += len(data)

    for data in units.values():
        f.write(data)
    
    lang = pofilename.strip(".po")
    
    with open(f"i18n_ymo_{lang}.hpp", "w") as wf:
        wf.write("static const unsigned char i18n_ymo_%s[] = { " % lang)
        wf.write(", ".join([hex(b) for b in f.getvalue()]))
        wf.write(" };")
        wf.close()

if __name__ == '__main__':
    po2ymohpp("zh_CN.po")
