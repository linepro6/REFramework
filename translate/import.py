from translate.storage import po
import csv

def main():
    csvf = csv.writer(open("zh_CN.csv", "w", encoding="utf-8-sig", newline=""))
    with open("zh_CN.po", "rb") as f:
        inputstore = po.pofile(f)
        for unit in inputstore.units:
            csvf.writerow([unit.source, unit.target])
        f.close()

main()