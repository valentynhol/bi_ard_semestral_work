# Hardwarová část

## 0. Příprava

### Naimportujte knihovnu `Keypad` a zkompilujte skript `/src/arduino/terminal.ino`.
### Poté připojte Arduino k počítači a nahrajte do něj tento skript.

## 1. Zvolení typu operace

### Po zapnutí se zobrazí menu, ze kterého lze vybrat typ operace.

**Například:**

```text
> Pay
Deposit
```

Šipka (>) označuje aktuálně vybranou možnost.

### Jsou dostupné 4 typy operací:

- **Pay** - zaplatit určitou částku
- **Deposit** - vložit peníze na účet
- **Balance** - kontrola zůstatku
- **Change PIN** - změna PIN kódu

### Ovládání v tomto stavu:

- **"<"** - pohyb nahoru
- **">"** - pohyb dolů
- **"0"** - potvrdit výběr

## 2. Vyplnění údajů

### Po výběru typu operace se terminál zeptá na číslo karty a PIN kód. Dále:

- #### V případě operace "Change PIN" se terminál ještě zeptá na nový PIN kód.

- #### V případě operací "Pay" a "Deposit" se zeptá na částku, kterou chcete zaplatit nebo vložit.

### Ovládání v tomto stavu:

- **"<"** - zpět
- **">"** - potvrdit
- **"0" až "9"** - číslice

## 3. Komunikace se "serverem"

### Po úspěšném vyplnění údajů Arduino vyčká na zpracování dat a zobrazí zprávu o chybě nebo úspěchu.

# "Server"

## 1. Příprava dat

### K uložení údajů o kartách, PIN kódech a zůstatcích slouží soubor `cards.csv`.

Data musí být ve formátu:

```text
"<card1>:<pin1>, <card2>:<pin2>, <card3>:<pin3>...", <balance>
```

### Kde:

- `<cardX>` jsou čísla karet
- `<pinX>` jsou příslušné PIN kódy
- `<balance>` je zůstatek na účtu, ke kterému jsou karty připojeny

### Příklad správného obsahu souboru `cards.csv`:

```text
"1111222233334444:1111",25000
"5555666677778888:2222",32000
"9999000011112222:1234",15000
"0000000000000000:0000,0000000000000001:0001",50000
"1234123412341234:4321",10000
"4444555566667777:1111,4444555566667778:2222",40000
"8888999900001111:3333",27000
"1010101010101010:1010,2020202020202020:2020,3030303030303030:3030",60000
```

### Poznámka

- Uvozovky ve formátu jsou **důležité** – bez nich program nemusí fungovat správně.

## 2. Příprava prostředí

### Aby program správně fungoval, je třeba nainstalovat potřebné knihovny.

V příkazové řádce spusťte:

```shell
pip install -r src/python/requirements.txt
```

## 3. Spuštění

Nejdříve zjistěte, na který sériový port je připojeno Arduino.

Poté spusťte:

```shell
python3 src/python/comm.py
```

Program se zeptá na port:

```text
> COM port: 
```

Zadejte název portu, například:

```text
> COM port: COM4
```

Pokud se objeví chybová hláška, zkontrolujte, zda je Arduino připojeno a zadali jste správný port:

```text
> COM port: COM4
> Error while opening
```