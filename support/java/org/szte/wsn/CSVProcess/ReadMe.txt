CSVProcess ReadMe
The CVSProcess reads the parameters from a configuration file.
The only argument we have to provide is the path of that configuration file.
If we run CVSProcess without any given argument, then it will looking for "ini.conf" at the running directory.

Form of the parameters:
Only one parameters per lines are allowed.
nameofparameter(case insensitive) = value
eg.: insertGlobal=true
or 
nameofparameter(case insensitive)="value"
eg.: separator=" "

The following parameters can be in the configuration file. We only have to write those which we want to modify.
The parameters which are missing from the configuration file will be set to the default value. 


separator
	String(character) value, TODO,  default is ";"

nodeIdSeparator
	String(character) value, separates header id from node id in header ,default is ":"

avgOutputFileName
	String value, name of the output file of the averages, default is "avgfile.csv"

insertGlobal
	Boolean value [true,false], determines whether we want to insert the global time column into the output file?? TODO ,default value is "true"
		
maxerror
	Integer value, determines the threshold in detecting tears in local timeline ??? TODO ,default value is 120

timeformat
	String value, determines the time display format, default is "yyyy.MM.dd/HH:mm:ss.SSS"

confFile
	String value, path of the converters configuration file, default is "convert.conf"

csvExt
	String value, extension of output files, when there are more files TODO, default is ".csv" 

startTime
	Long value [min, any long value], determines the starting time of the merging in the global file, default is "min" 

endTime
	Long value [max, any long value], determines the ending time of the merging in the global file, default is "max" 

timeWindow
	Long value, determines the length of the average calculation in millisecundums, default is 900000

timeType
	String value???[start,end,middle], determines whether the starting, ending or middle of the time period
	 should be displayed at the averages, default is start

If we have different structures in our data set, we can process them separatly. 
To do so we have to set some structure related parameters.
After the global parameters we have to place structure keyword and the name of that structure.   
The parameters of the structure can follow this line. We can repeat it with every structure. 
eg.:structture sample1


localColumn
	Integer value, indicates the column of the local time TODO, default?? is 4
	
globalColumn
	Integer value, indicates the column of the global time TODO, default?? is 5
	
dataColumns
	List of Integer values separated by ',' ,shows which are the data columns TODO, default?? is "1,2,3,4" 

outputfile
	String value, path of the structure's output file, default is "global.csv"
