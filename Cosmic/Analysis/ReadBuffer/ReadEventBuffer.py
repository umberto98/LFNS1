import Tools

nmodules = 9  # numero di moduli usati nell'esperimento
m_bytes = [4, 4, 4, 4, 2, 2, 2, 2, 1]  # numero di byte per ogni modulo
m_chaneff = [16, 12, 16, 16, 8, 12, 12, 2, 16]  # numero di canali fisici per ogni modulo
m_chanlog = [16, 12, 16, 16, 8, 16, 16, 8, 16]  # numero di canali scritti sul file
m_stop = []  # numero linea di stop

# calcolo la riga di stop
temp = nmodules * 4
for i in range(0, nmodules):
    temp += int(m_chanlog[i] * m_bytes[i] / 16)
    m_stop.append(temp)

# Lettura del file vero e proprio
file = open("notte_weekend/weekend.out", "r")
f_out = open("notte_weekend/weekend.txt", "w")
# f_out.write("Quello che vogliamo")

line = 0  # variabile che mi tiene traccia della riga di lettura
chantemp = 0  # variabile che mi tiene traccia del canale a cui sono arrivato nella lettura
nbytes = 0  # variabile che mi dice quanti bytes sono stati dedicati all'evento
nlines = 0  # mi dice quante righe occupa l'evento
chanactscaler = [0]
chanactadc = [10, 11]
chanactpu = [0]
chanactcs = [15]
chanacttdc = [6]

f_out.write(
    "Event number - ErrMask - Clock Scaler - Scaler - Scaler inhibited - TDC - ADC Ch {0} - ADC Ch {1} - PU\n".format(
        str(chanactadc[0]), str(chanactadc[1])))

# for su tutte le righe
for row in file:
    if line == 0:
        nbytes = Tools.firstline(row, f_out)  # setto in numero di bytes per evento
        nlines = int(nbytes / 16)  # setto il numero di linee per evento
    elif line % nlines == 0:
        nbytes = Tools.firstline(row, f_out)  # stessa cosa delle due righe precedenti, ma solo per il secondo evento
        nlines = int(nbytes / 16)

    # leggo la descrizione dei moduli
    elif 0 < line <= nmodules * 4:
        Tools.module(row, line)

    # leggo i dati dei moduli
    elif nmodules * 4 < line % nlines <= m_stop[0]:
        # if line % nlines == nmodules * 4 + 1:
        #     print("\nModulo 1:")
        chantemp = Tools.data_32(row, chantemp, m_chaneff[0], m_chanlog[0], f_out, chanactcs)

    elif m_stop[1] < line % nlines <= m_stop[2]:
        # if line % nlines == m_stop[1] + 1:
        #     print("\nModulo 3:")
        chantemp = Tools.data_32(row, chantemp, m_chaneff[2], m_chanlog[2], f_out, chanactscaler)

    elif m_stop[2] < line % nlines <= m_stop[3]:
        # if line % nlines == m_stop[2] + 1:
        #     print("\nModulo 4:")
        chantemp = Tools.data_32(row, chantemp, m_chaneff[3], m_chanlog[3], f_out, chanactscaler)

    elif m_stop[3] < line % nlines <= m_stop[4]:
        # if line % nlines == m_stop[3] + 1:
        #     print("\nModulo 5:")
        chantemp = Tools.data_16(row, chantemp, m_chaneff[4], m_chanlog[4], f_out, chanacttdc)

    elif m_stop[5] < line % nlines <= m_stop[6]:
        # if line % nlines == m_stop[5] + 1:
        #     print("\nModulo 7:")
        chantemp = Tools.data_16(row, chantemp, m_chaneff[6], m_chanlog[6], f_out, chanactadc)

    elif m_stop[6] < line % nlines <= m_stop[7]:
        # if line % nlines == m_stop[6] + 1:
        #     print("\nModulo 8:")
        chantemp = Tools.data_16(row, chantemp, m_chaneff[7], m_chanlog[7], f_out, chanactpu)

    elif line % nlines == m_stop[8]:
        f_out.write("\n")
    # ho finito di leggere la riga
    line += 1  # incremento il numero di riga letta

# chiudo il file
file.close()
f_out.close()

# moduli non usati

# LeCroy 2251 - scaler - 12ch x 32 (24)bit
# elif m_stop[0] < line % nlines <= m_stop[1]:
#     if line % nlines == m_stop[0] +1:
#         print("\nModulo 2:")
#     chantemp = Tools.data_32(row, chantemp, m_chaneff[1], m_chanlog[1])

# 2249A - adc - 12ch x 16 (10)bit + 8 Bytes padding
# elif m_stop[4] < line % nlines <= m_stop[5]:
#     if line % nlines == m_stop[4] + 1:
#         print("\nModulo 6:")
#     chantemp = Tools.data_16(row, chantemp, m_chaneff[5], m_chanlog[5])

# C211 - programmable delay - 16ch x 8bit
# elif m_stop[7] < line % nlines <= m_stop[8]:
#     if line % nlines == m_stop[7] +1:
#         print("\nModulo 9:")
#     chantemp = Tools.data_8(row, chantemp, m_chaneff[8], m_chanlog[8])

# Fine evento
# else:
#     print("\nEvento letto completamente")
