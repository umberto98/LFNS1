import Tools

nmodules = 5  # numero di moduli usati nell'esperimento
m_bytes = [4, 4, 2, 2, 4]  # numero di byte per ogni modulo
m_chaneff = [16, 12, 8, 12, 16]  # numero di canali fisici per ogni modulo
m_chanlog = [16, 12, 8, 16, 16]  # numero di canali scritti sul file
m_stop = []  # numero linea di stop

# calcolo la riga di stop
temp = nmodules * 4
for i in range(0, nmodules):
    temp += int(m_chanlog[i] * m_bytes[i] / 16)
    m_stop.append(temp)

# Lettura del file vero e proprio
file = open("example.txt", "r")
line = 0  # variabile che mi tiene traccia della riga di lettura
chantemp: int = 0  # variabile che mi tiene traccia del canale a cui sono arrivato nella lettura
nbytes = 0  # variabile che mi dice quanti bytes sono stati dedicati all'evento
nlines = 0  # mi dice quante righe occupa l'evento

# for su tutte le righe
for row in file:

    if line == 0:
        nbytes = Tools.firstline(row)  # setto in numero di bytes per evento
        nlines = int(nbytes / 16)  # setto il numero di linee per evento
    elif line % nlines == 0:
        nbytes = Tools.firstline(row)  # stessa cosa delle due righe precedenti, ma solo per il secondo evento
        nlines = int(nbytes / 16)

    # leggo la descrizione dei moduli
    elif 0 < line % nlines <= nmodules * 4:
        Tools.module(row, line)

    # leggo i dati dei moduli
    elif nmodules * 4 < line % nlines <= m_stop[0]:
        if line % nlines == 21:
            print("\nModulo 1:")
        chantemp = Tools.data_32(row, chantemp, m_chaneff[0], m_chanlog[0])

    elif m_stop[0] < line % nlines <= m_stop[1]:
        if line % nlines == 25:
            print("\nModulo 2:")
        chantemp = Tools.data_32(row, chantemp, m_chaneff[1], m_chanlog[1])

    elif m_stop[1] < line % nlines <= m_stop[2]:
        if line % nlines == 28:
            print("\nModulo 3:")
        chantemp = Tools.data_16(row, chantemp, m_chaneff[2], m_chanlog[2])

    elif m_stop[2] < line % nlines <= m_stop[3]:
        if line % nlines == 29:
            print("\nModulo 4:")
        chantemp = Tools.data_16(row, chantemp, m_chaneff[3], m_chanlog[3])

    elif m_stop[3] < line % nlines <= m_stop[4]:
        if line % nlines == 31:
            print("\nModulo 5:")
        chantemp = Tools.data_32(row, chantemp, m_chaneff[4], m_chanlog[4])

    # ho finito di leggere l'evento
    else:
        print("\nEvento letto completamente")
    line += 1  # incremento il numero di riga letta

# chiudo il file
file.close()
