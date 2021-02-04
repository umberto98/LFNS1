############### funzioni usate nel codice ##########################

# Funzione per leggere le righe in ASCII
def module(row, line):
    data = row.split(": ", 2)  # divido in indice di riga e dati
    seq = data[1].replace(" ", "")  # rimuovo gli spazi dai dati
    byte = [seq[j:j + 2] for j in range(0, 32, 2)]  # creo una lista di byte(8bit)
    # loop sui byte della riga
    for l in range(0, 15):
        if 128 > int(byte[l], 16) > 31:  # controllo che il byte sia un carattere ascii
            char = (bytes.fromhex(byte[l])).decode("ASCII")  # converto il byte in carattere
            print(char, end='')
    if line % 4 == 0:
        print()  # vado a capo ogni 4 righe = lunghezza della def del modulo


# Funzione per leggere le righe con dati da 8 bit
# channeltemp è il canale che leggo, channeleff è il numero di canali fisici per modulo,
# channellog è il numero di canali che metto nel testo per simmetria
def data_8(row, chantemp, chaneff, chanlog):
    data = row.split(": ", 2)  # divido in indice di riga e dati
    seq = data[1].replace(" ", "")  # rimuovo gli spazi dai dati
    byte = [seq[j:j + 2] for j in range(0, 32, 2)]  # creo una lista di byte(8bit)
    # loop sui byte della riga
    for i in range(0, 17):
        if chantemp < chaneff:
            print("Channel", chantemp, ":", int(byte[i], 16))  # converto tutti i miei canali fisici
            chantemp += 1
            if chantemp == chanlog:  # ho letto tutti i canali sul file
                return 0
    return chantemp

# Funzione per leggere le righe con dati da 8 bit
# channeltemp è il canale che leggo, channeleff è il numero di canali fisici per modulo,
# channellog è il numero di canali che metto nel testo per simmetria
def data_16(row, chantemp, chaneff, chanlog):
    word = row.split(" ", 9)  # divido in word la riga
    # faccio un loop sulle word (16 bit) significative
    for i in range(1, 9):
        if chantemp < chaneff:
            print("Channel", chantemp, ":", int(word[i], 16))  # converto tutti i miei canali fisici
        chantemp += 1
        if chantemp == chanlog:  # ho letto tutti i canali sul file
            return 0
    return chantemp


# Funzione per leggere le righe con dati da 16 bit
# channeltemp è il canale che leggo, channeleff è il numero di canali fisici per modulo,
# channellog è il numero di canali che metto nel testo per simmetria
def data_32(row, chantemp, chaneff, chanlog):
    data = row.split(": ", 2)  # divido in indice di riga e dati
    seq = data[1].replace(" ", "")  # rimuovo gli spazi dai dati
    dworld = [seq[j:j + 8] for j in range(0, 32, 8)]  # creo una lista di double world(32 bit)
    # forlooop sulle dworld (32 bit) presenti sulla riga
    for i in range(0, 4):
        if chantemp < chaneff:  # converto tutti i miei canali fisici
            print("Channel", chantemp, ":", int(dworld[i], 16))
        chantemp += 1
        if chantemp == chanlog:  # ho letto tutti i canali sul file
            return 0
    return chantemp


# Funzione che legge la prima riga
def firstline(row):
    data = row.split(": ", 2)  # divido in indice di riga e dati
    seq = data[1].replace(" ", "")  # rimuovo gli spazi dai dati
    nbytes = seq[0:4]  # mi restituisce il numero di byte per evento
    event = seq[4:12]  # mi restituisce il numero dell'evento
    errmask = seq[12:16]  # mi resituise lo status dei moduli per quell'evento
    # stampo tutto
    print("Numero evento:", int(event, 16))
    print("Numero bytes per evento:", int(nbytes, 16))
    print("Status moduli:", bin(int(errmask, 16))[2:])
    print("\n")
    return int(nbytes, 16)
