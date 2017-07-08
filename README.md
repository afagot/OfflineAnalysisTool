# Offline analysis tool of the GIF++ DAQ
#### Author: Alexis Fagot
#### email : alexis.fagot@ugent.be
#### Tel.: +32 9 264 65 69
#### Mobile: +32 4 77 91 61 31
#### Mobile: +33 6 66 89 02 90

## Compilation

To be able to use the offline analysis tool, it is mandatory to have previously installed **[ROOT](https://root.cern.ch/drupal/content/installing-root-source)**. ROOT can be cloned from the git HTTP repository :

    git clone http://root.cern.ch/git/root.git

Then follow the intalation steps on this page : **[HERE](https://root.cern.ch/drupal/content/installing-root-source)**.

To compile the offline analysis project for the first time, simply do :
```bash
mkdir build; cd build; cmake ..; make install
```
Only do `make install` in the `build/` folder aftwerwards.

## Usage

You first have to fill a json configuration file with the path to the data you want to analyse, the number of strips of the electronic and a varierty of cut you want to apply. Full list of options are detailed in the [Configuration](#configuration) section. You can find an example file in the [`config/`](https://raw.githubusercontent.com/afagot/OfflineAnalysisTool/alexis/config/) folder.
You then execute the following command : 
```bash
./bin/OfflineAnalysis /path/to/config/file [TimeEarliestSignals] [TimeLatestSignals]
```
Parameters between `[]` are optional, if given they will replace the one defined in the configuration file.

The program will create three folders `DAT/`, `CSV/` and `ROOT/` in the folder containing the data, then move accordingly the analysed file in their respective folder.
 
Default values for `TriggerWindowWidth`, `TimeEarliestSignals` and `TimeLatestSignals` are 1000, 0 and 1000 but are not focussed on the cosmic muon peak. `TriggerWindowWidth` correspond to the value set in the DAQ software during the data acquisition.  

To have a good estimation of `TimeEarliestSignals` and `TimeLatestSignals`, first run the analysis with the default value, then :
```c++
root -l
new TBrowser
```
and look into the histogram called `TimeProfile`. Zoom on the cosmics pic to get the first and last bin values. To do so, the first step is to place the mouse onto the axis at the level of the units :

![Mouse on axis](https://raw.githubusercontent.com/afagot/OfflineAnalysisTool/alexis/img/step1.png "Mouse on axis")

The second step consists in selecting using the left click of the mouse the range you want to zoom in. Repeat that step as many times as needed (you cannot zoom on tooo small ranges...) :

![Range selection](https://raw.githubusercontent.com/afagot/OfflineAnalysisTool/alexis/img/step2.png "Range selection")

Finally, read the value of the first bin and last bin on the x-axis :

![Bins reading](https://raw.githubusercontent.com/afagot/OfflineAnalysisTool/alexis/img/step3.png "Bins reading")

Once you have all the needed values, you can either adjust the value in the configuration file or at the command line.


You can now print the efficiency and cluster size with :
```bash
cat /path/to/data/folder/CSV/RESULT_[datafile].csv
```

# Configuration
**TODO: add more options**  
Configuration file uses [Json](http://www.json.org/) format using the syntax `{"Key" : Value}`.
The value can be a string, an int, a list, or Json object (a new set of `{"Key":Value}`).  
Here is a description of all available option: (in `[]` their default value)

  - `"DataPath"` [`/path/to/data/folder/`] : String __Need to finish with a '/'__ 
  - `"NStrips"` [`16`] : Int, number of strips on the electronic used for data taking
  - `"XStripMin"` [`-1`]  : Int, cut on lowest strip value along X
  - `"XStripMax"` [`100`] : Int, cut on max strip value along X
  - `"YStripMin"` [`-1`]  : Int, cut on lowest strip value along Y
  - `"YStripMax"` [`100`] : Int, cut on max strip value along Y
  - `"XNHitMax"` [`500`]  : Int, cut on max multiplicity along X
  - `"YNHitMax"` [`500`]  : Int, cut on max multiplicity along Y 
  - `"StartTimeCut"` [`0`]  : FLoat, Timing cut to select muon peak (low) 
  - `"EndTimeCut"` [`1000`] : FLoat, Timing cut to select muon peak (high) 
