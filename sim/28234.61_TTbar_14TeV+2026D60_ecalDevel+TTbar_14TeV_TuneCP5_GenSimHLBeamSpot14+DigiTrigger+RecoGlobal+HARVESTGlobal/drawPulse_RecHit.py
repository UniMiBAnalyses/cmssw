import ROOT
from DataFormats.FWLite import Events, Handle


from optparse import OptionParser
parser = OptionParser()
parser.add_option("--ieta", dest="ieta", default = -99, help = "ieta", type = 'int')
parser.add_option("--iphi", dest="iphi", default = -99, help = "iphi", type = 'int')

parser.add_option("--event",      dest="whichevent",   default = -1, help = "Which event to plot",   type = 'int')
parser.add_option("--minevent",   dest="minevent",   default = 0, help = "First event to plot",   type = 'int')
parser.add_option("--maxevent",   dest="maxevent",   default = 10, help = "Last event to plot",    type = 'int')

(options, args) = parser.parse_args()

events = Events('step3.root')

mg = ROOT.TMultiGraph()

# EBDigiCollectionPh2                   "simEcalUnsuppressedDigis"   ""                "DIGI"
handleEB  = Handle ('EBDigiCollectionPh2')
labelEB = ("simEcalUnsuppressedDigis", "", "DIGI")

# edm::SortedCollection<EcalUncalibratedRecHit,edm::StrictWeakOrdering<EcalUncalibratedRecHit> >    "ecalUncalibRecHitPhase2"   "EcalUncalibRecHitsEB"   "RECO"    
handleRechitEB  = Handle ('edm::SortedCollection<EcalUncalibratedRecHit,edm::StrictWeakOrdering<EcalUncalibratedRecHit> >')
labelRechitEB = ("ecalUncalibRecHitPhase2","EcalUncalibRecHitsEB")


whichevent = options.whichevent
minevent = options.minevent
maxevent = options.maxevent

if whichevent != -1 :
  print " pulse only the event: ", whichevent
  minevent = whichevent
  maxevent = whichevent+1
  
num_events = 0

map_EB = {}

for line in open("EB.txt"):
    li = line.strip()
    splitLine = line.split()
    if not li.startswith("#"):
        map_EB [ ( int(splitLine[4]) , int(splitLine[3]) ) ] = ( int(splitLine[2]), int(splitLine[0]) )

ieta = options.ieta
iphi = options.iphi

ideb = -1
cmssw_id = -1
maxADC = 0

if ieta != -99 :
  ideb     = map_EB [ (ieta, iphi) ] [0]
  cmssw_id = map_EB [ (ieta, iphi) ] [1]

for event in events:
  num_events += 1 
  
  if (num_events <= maxevent) and (num_events>minevent) :

    event.getByLabel (labelEB, handleEB)
    ebdigi = handleEB.product()
  
    event.getByLabel (labelRechitEB, handleRechitEB)
    ebrechits = handleRechitEB.product()

    gr_amplitude = ROOT.TGraph()
    gr_amplitude.Clear()

    gr = ROOT.TGraph()
    gr.Clear()

    amplitude = -99
    
    # EB
    if ieta != -99 :
      for isample in range (16) :    
        #                                                             0xFFF = 4095
        gr.SetPoint(isample,isample*6.25,  ((ebdigi[ideb][isample]) & 0xFFF) )

      for ebrechit in ebrechits :
        if ebrechit.id()() == cmssw_id:
          print " found EB"
          amplitude = ebrechit.amplitude()
          print " amplitude = ", ebrechit.amplitude()
                
    #---- pulse

    color = ROOT.kBlue

    gr.SetMarkerColor(color)
    gr.SetLineColor  (color)

    gr.SetMarkerSize(3)
    gr.SetMarkerStyle(20)

    mg.Add(gr)

    #---- amplitude

    gr_amplitude.SetPoint(0, 5*6.25, amplitude )  #l'ampiezza massima e nel campionamento numero 5

    gr_amplitude.SetMarkerColor(ROOT.kMagenta)
    gr_amplitude.SetLineColor  (ROOT.kMagenta)

    gr_amplitude.SetMarkerSize(3)
    gr_amplitude.SetMarkerStyle(34)

    mg.Add(gr_amplitude)
   

cc = ROOT.TCanvas("cc","pulse", 800, 600) 
mg.Draw("apl")
mg.GetXaxis().SetTitle("time [ns]")
mg.GetYaxis().SetTitle("ADC count")

mg.SaveAs("mg.root") 