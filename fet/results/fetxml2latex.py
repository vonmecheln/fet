#!/usr/bin/python

# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#Received from Vimal Joseph on 7 June 2004


import xml.parsers.expat, sys 

class MyXML: 
    Parser = "" 
    timetable={}
    day=""
    hour=""
    hours=[]
    days=[]
    which_tt=""
    # Prepare for parsing 
    def __init__(self, xml_filename): 
        assert xml_filename != "" 
        
        self.xml_filename = xml_filename
        self.Parser = xml.parsers.expat.ParserCreate(  ) 
 
        self.Parser.CharacterDataHandler = self.handleCharData 
        self.Parser.StartElementHandler = self.handleStartElement 
        self.Parser.EndElementHandler = self.handleEndElement 
 
    # Parse the XML file 
    def parse(self): 
        try: 
            xml_file = open(self.xml_filename, "r")
        except: 
            print "ERROR: Can't open XML file %s"%self.xml_filename 
            raise 
        else: 
            try: self.Parser.ParseFile(xml_file) 
            finally: xml_file.close(  ) 
 
    # to be overridden by implementation-specific methods 
    def handleCharData(self, data): pass
    
    def handleStartElement(self, name, attrs):
        if (name=="Students_Timetable") or (name=="Teachers_Timetable"):
            self.which_tt=name
            print "\\documentclass[a4paper,12pt]{article}"
            print "\\usepackage{anysize}"
            print "\\marginsize{1in}{1in}{1in}{1in}"
            print "\\begin{document}"
            
        if (name=="Subgroup") and (self.which_tt=="Students_Timetable"):
            self.timetable["class"]=attrs['name']
            self.days=[]
        if (name=="Teacher") and (self.which_tt=="Teachers_Timetable"):
            self.timetable["teacher"]=attrs['name']
            self.days=[]
            
        if name=="Day" :
            self.day=attrs['name']
            self.days.append(self.day)
            self.hours=[]
        if name=="Hour" :
            self.hour=attrs['name']
            self.hours.append(self.hour)
        if name=="Subject" :
            self.timetable[self.day+self.hour]=attrs['name']
        if name=="Students" :
            self.timetable[self.day+self.hour]=attrs['name']
            
    def handleEndElement(self, name):
        if (name=="Subgroup") and (self.which_tt=="Students_Timetable"):
            
            print "\\begin{tabular}{|c|",
            for i in self.days:
                print "c|", 
            print "}"
            print "\\hline"
            print "\\multicolumn{",len(self.days)+1,"}{|c|}{", self.timetable["class"],"}\\\\"
            print "\\hline"
            print "no.",
            for i in self.days:
                print "&",i,
            print "\\\\"
            print "\\hline"
            for i in self.hours:
                print i,
                for j in self.days:
                    if j+i in self.timetable.keys():
                        print "&", self.timetable[j+i], 
                    else:
                        print "&", "---",
                print "\\\\ \n"
            self.timetable={}
            print "\\hline"
            print "\\end{tabular}\n\n"
            print "\ \\\\\n\n"

        if (name=="Teacher") and (self.which_tt=="Teachers_Timetable"):
            print "\\begin{tabular}{|c|",
            for i in self.days:
                print "c|", 
            print "}"
            print "\\hline"
            print "\\multicolumn{",len(self.days)+1,"}{|c|}{", self.timetable["teacher"],"}\\\\"
            print "\\hline"
            print "no." 
            for i in self.days:
                print "&",i,
            print "\\\\"
            
            print "\\hline"
            for i in self.hours:
                print i,
                for j in self.days:
                    if j+i in self.timetable.keys():
                        print "&", self.timetable[j+i], 
                    else:
                        print "&", "---",
                print "\\\\ \n"
            self.timetable={}
            print "\\hline"
            print "\\end{tabular}\n\n"
            print "\ \\\\\n\n"
        
        if (name=="Students_Timetable") or (name=="Teachers_Timetable"):
            print "\\end{document}"
            

def print_usage():
    print "fetxml2latex.py [<in_file>]"
    print "where in_file is teacherstimetable.xml or studentstimetable.xml"
    print "eg. fetxml2latex.py teacherstimetable.xml > teachers.tex"
    sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) == 2:
        infile = sys.argv[1]
    else:
        print_usage()
        
    xmp = MyXML(infile)
    xmp.parse()
