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

if ieta != -99 :
  ideb     = map_EB [ (ieta, iphi) ] [0]
  cmssw_id = map_EB [ (ieta, iphi) ] [1]

for event in events:
  num_events += 1 
  
  if (num_events <= maxevent) and (num_events>minevent) :
  
    event.getByLabel (labelEB, handleEB)
    ebdigi = handleEB.product()

    dataFrame = EcalDataFrame_Ph2 (ebdigi)

    # gr = ROOT.TGraph()
    # gr.Clear()
    
    # # EB
    # if ieta != -99 :
    #   for isample in range (16) :    
    #     #                                                           0xFFF = 4095
    #     gr.SetPoint(isample,isample*6.25,  ((ebdigi[ideb][isample]) & 0xFFF) )
        
    #     ADC = ((ebdigi[ideb][isample]) & 0xFFF)

    #     print ADC 
              
    #---- pulse
    
#     color = ROOT.kBlue

#     gr.SetMarkerColor(color)
#     gr.SetLineColor  (color)
    
#     gr.SetMarkerSize(3)
#     gr.SetMarkerStyle(20)

#     mg.Add(gr)


# print " ieta = ", ieta, "   iphi = ", iphi, " ==> max ADC count = ", maxADC

# cc = ROOT.TCanvas("cc","pulse", 800, 600) 
# mg.Draw("apl")
# mg.GetXaxis().SetTitle("time [ns]")
# mg.GetYaxis().SetTitle("ADC count")

# mg.SaveAs("mg.root") 
