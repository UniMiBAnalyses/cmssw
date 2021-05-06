import ROOT
from DataFormats.FWLite import Events, Handle

events = Events('step3.root')

# EBDigiCollectionPh2                   "simEcalUnsuppressedDigis"   ""                "DIGI"

handleEB  = Handle ('EBDigiCollectionPh2')
labelEB = ("simEcalUnsuppressedDigis", "", "DIGI")

minevent = 0
maxevent = 10
  
num_events = 0

map_EB = {}

for line in open("EB.txt"):
    li = line.strip()
    splitLine = line.split()
    if not li.startswith("#"):
        map_EB [ ( int(splitLine[4]) , int(splitLine[3]) ) ] = ( int(splitLine[2]), int(splitLine[0]) )

ieta = -85
iphi = 1

ideb = -1

cmssw_id = -1


for event in events:
  maxADC = 0
  num_events += 1

  print "num_events = ", num_events

  ieta = -85

  while ieta < 86:
    iphi = 1
    
    while iphi < 361:
      ideb     = map_EB [ (ieta, iphi) ] [0]
      cmssw_id = map_EB [ (ieta, iphi) ] [1]

      # print " ieta = ", ieta, ", iphi = ", iphi

      # num_events = 1
      
      if (num_events <= maxevent) and (num_events>minevent) :

        event.getByLabel (labelEB, handleEB)
        ebdigi = handleEB.product()
        
        # EB
        for isample in range (10) :
          
          ADC = ((ebdigi[ideb][isample]) & 0xFFF)

          if ADC > 100:
            print "Ho trovato un conteggio ADC maggiore di 100 = ", ADC, " con:  ieta = ", ieta, "   iphi = ", iphi
            print "isample = ", isample
            print "ideb = ", ideb, "\n"

          if ADC > maxADC:
            maxADC = ADC

      iphi = iphi + 1

    ieta = ieta + 1
    if ieta == 0:
      ieta = ieta + 1
    if ieta == 86:
      print "Il massimo conteggio ADC che ho trovato nell'evento ", num_events, " e' ", maxADC