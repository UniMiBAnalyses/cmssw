import ROOT
from DataFormats.FWLite import Events, Handle


from optparse import OptionParser
parser = OptionParser()
parser.add_option("--event",      dest="whichevent",   default = -1, help = "Which event to plot",   type = 'int')

(options, args) = parser.parse_args()

events = Events('step3.root')

# edm::SortedCollection<EcalUncalibratedRecHit,edm::StrictWeakOrdering<EcalUncalibratedRecHit> >    "ecalUncalibRecHitPhase2"   "EcalUncalibRecHitsEB"   "RECO"    
handleRechitEB  = Handle ('edm::SortedCollection<EcalUncalibratedRecHit,edm::StrictWeakOrdering<EcalUncalibratedRecHit> >')
labelRechitEB = ("ecalUncalibRecHitPhase2","EcalUncalibRecHitsEB")


whichevent = options.whichevent

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

for event in events:
  num_events += 1 
  
  if (num_events <= maxevent) and (num_events>minevent) :

    event.getByLabel (labelRechitEB, handleRechitEB)
    ebrechits = handleRechitEB.product()

    # histo_amp = ROOT.TH1F("histo_amp", "Amplitude Spectrum", 200, 0, 200)
    histo_amp = ROOT.TH1F("histo_amp", "Amplitude Spectrum", 110, 0, 11000)

    amplitude = -99

    for ebrechit in ebrechits :
        amplitude = ebrechit.amplitude()
        histo_amp.Fill(amplitude)

cc = ROOT.TCanvas("cc","Amplitude", 800, 600) 
histo_amp.GetXaxis().SetTitle("Amplitude")
histo_amp.GetYaxis().SetTitle("dN/dx")
histo_amp.SetLineColor(ROOT.kMagenta)
histo_amp.SetLineWidth(2)
histo_amp.Draw("HIST")
cc.SetLogy()
cc.Print("histo_amp.root") 