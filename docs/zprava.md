Většinou se mi podařilo realizovat vše, co jsem si naplánoval. Jsem spokojený se svou prací, i když není dokonalá.

Jediné věci, které nejsou optimálně vyřešené, jsou:
- použití klávesnice pro zadávání čísla karty
- nebezpečná komunikace mezi terminálem a "serverem“

### Problémy, které se objevily:
1) Neměl jsem k dispozici RFID čtečku/zapisovačku, takže jsem to musel vyřešit pomocí klávesnice. V důsledku toho byla implementace i testování o něco složitější.
2) Způsob ukládání dat. Měl jsem nápad použít Arduino Esplora, protože má slot na SD kartu, ale nemohl jsem k němu připojit klávesnici, takže jsem to nakonec vyřešil pomocí sériové komunikace s Python skriptem.
