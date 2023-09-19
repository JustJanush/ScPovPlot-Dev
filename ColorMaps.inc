/****h* ScPovPlot3D/ColorMaps.inc
* PURPOSE
*   New file defining some color maps esp. for BPutchSurf.inc, Potential.inc, VectorField.inc, ...
*   There still is a lot TODO....
*    |html <hr width=50% align="left">
* VERSION
*  4.0.2, tested on PovRay 3.7.
* AUTHOR
*  Janusz Opi³a‚a Ph.D.
*   jmo{at}agh.edu.pl, janusz.opila{at}gmail.com
*   Dept. of Applied Informatics,
*   href:https://www.facebook.com/KatedraInformatykiStosowanejWZAGH/
*   href:http://kis.zarz.agh.edu.pl/
*   AGH University of Science & Technology, Cracow, Poland href:http://www.zarz.agh.edu.pl/English/index.asp
*   Maintained by Janusz Opi³a‚a Ph.D.
*   Homepage: href:http://scpovplot3d.sourceforge.net
* COPYRIGHT
*  GNU GPL v.3 License
*  (c) 2012-now by Janusz Opi³a‚a Ph.D.
*  AGH University of Science and Technology
*
****
  end of RoboDoc comment
****/

#ifndef(ColorMaps_INC)
#declare ColorMaps_INC="2020-05-22"; // last modification date
// common includes & definitions
#version 3.7;
#declare _FileName   = "ColorMaps.INC";
#declare _FileVerMaj = "4.0.2";
#declare _FileVerMin = ColorMaps_INC;

#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

#ifndef (Debug) // Just 4 safety
   #declare Debug=0;
#end

#include "colors.inc"
#include "textures.inc"
#include "glass.inc"
#include "metals.inc"

// default, module level definitions
#declare Filter=.0;
#declare Warstwy=yes;
#declare Effects=false;
#declare LevCol=Black;

#declare ReversedColorMap=color_map {
           [0.1 color Red]
           [0.3 color Yellow]
           [0.5 color Blue]
           [0.6 color Green]
           [0.8 color Cyan]
           [1.0 color rgbf <1, 0.2, 0.2, 1.0> ]
           }; // color_map

#declare Rainbow_Colors=
    color_map {
      [0.0   color Magenta]
      [0.33  color Yellow]
      [0.67  color Cyan]
      [1.0   color Magenta]
    };

#macro SetWarstwyOff()
  #declare Warstwy=no;
#end

#macro SetWarstwyOn()
  #declare Warstwy=yes;
#end

#macro SetFilter(_F)
  #declare Filter = _F;
#end

#declare BW_Map =
  color_map {
     [0.000 color rgb 0.05  filter Filter]
     [0.100 color rgb 0.00  filter Filter]

     [0.100 color rgb 0.15  filter Filter]
     [0.200 color rgb 0.10  filter Filter]

     [0.200 color rgb 0.20  filter Filter]
     [0.300 color rgb 0.15  filter Filter]

     [0.300 color rgb 0.25  filter Filter]
     [0.400 color rgb 0.20  filter Filter]

     [0.400 color rgb 0.30  filter Filter]
     [0.500 color rgb 0.25  filter Filter]

     [0.500 color rgb 0.35  filter Filter]
     [0.600 color rgb 0.30  filter Filter]

     [0.600 color rgb 0.40  filter Filter]
     [0.700 color rgb 0.35  filter Filter]

     [0.700 color rgb 0.45  filter Filter]
     [0.800 color rgb 0.40  filter Filter]

     [0.800 color rgb 0.45  filter Filter]
     [0.900 color rgb 0.40  filter Filter]

     [0.900 color rgb 0.55  filter Filter]
     [1.000 color rgb 0.45  filter Filter]
}; //color_map

#declare BW_Map10 =
  color_map { BW_Map
};


#declare BW_Map5 =
  color_map {
     [0.000 color rgb 0.20  filter Filter]
     [0.200 color rgb 0.15  filter Filter]

     [0.200 color rgb 0.35  filter Filter]
     [0.400 color rgb 0.25  filter Filter]

     [0.400 color rgb 0.55  filter Filter]
     [0.600 color rgb 0.45  filter Filter]

     [0.600 color rgb 0.75  filter Filter]
     [0.800 color rgb 0.65  filter Filter]

     [0.800 color rgb 0.85  filter Filter]
     [1.000 color rgb 0.75  filter Filter]
}; //color_map


#declare Gray_Map =
  color_map {
     [0.000 color rgb 0.05  filter Filter]
     [0.100 color rgb 0.10  filter Filter]
     [0.200 color rgb 0.20  filter Filter]
     [0.300 color rgb 0.20  filter Filter]
     [0.400 color rgb 0.30  filter Filter]
     [0.500 color rgb 0.40  filter Filter]
     [0.600 color rgb 0.50  filter Filter]
     [0.700 color rgb 0.60  filter Filter]
     [0.800 color rgb 0.70  filter Filter]
     [0.900 color rgb 0.80  filter Filter]
     [1.000 color rgb 0.95  filter Filter]
}; //color_map


#declare Cartographic_Colors =
  color_map {
     [0.000 color NewMidnightBlue  filter Filter]
     [0.100 color Blue filter Filter]
     [0.148 color Cyan filter Filter]
     [0.245 color Green filter Filter]
    #if ( Warstwy )
         [0.245 color LevCol ]
        [0.250 color LevCol ]
         [0.250 color Green filter Filter]  //  yellow lemon
    #end

    [0.400 color rgbf< 0.75, 1.0, 0.0, Filter>]     //  yellow lemon

    [0.495 color Yellow  filter Filter]
    #if ( Warstwy )
         [0.495 color LevCol ]
        [0.500 color LevCol ]
         [0.500 color Yellow filter Filter]
    #end

   [0.545 color Gold  filter Filter]
   [0.575 color Orange  filter Filter]

   [0.765 color OrangeRed  filter Filter]
   #if ( Warstwy )
        [0.765 color LevCol ]
         [0.770 color LevCol ]
         [0.770  color OrangeRed filter Filter]
    #end
    [0.875 color Firebrick filter Filter]     //  very dark red
    [0.990 color rgbf< 1.0, 0.0, 0.0, Filter>]
}; //color_map

#declare Cartographic_Colors2=  
// internal data
// #declare Filter=.65;          
// #declare Warstwy=yes;     
// #declare Effects=false;
// #declare LevCol=Black;    
  color_map	{
  	 [0.000 color NewMidnightBlue  filter Filter]
  	 [0.100 color Blue filter Filter]  
  	 [0.148 color Cyan filter Filter]  
  	 [0.245 color Green filter Filter]
    #if ( Warstwy )
     	 [0.245 color LevCol ]
        [0.250 color LevCol ]        	
     	 [0.250 color Green filter Filter]  //  yellow lemon
    #end        	

    [0.400 color rgbf< 0.75, 1.0, 0.0, Filter>]     //  yellow lemon

    [0.495 color Yellow  filter Filter]
    #if ( Warstwy )
     	 [0.495 color LevCol ]
        [0.500 color LevCol ]        	
     	 [0.500 color Yellow filter Filter]
    #end        	

   [0.545 color Gold  filter Filter]
   [0.575 color Orange  filter Filter]

   [0.765 color OrangeRed  filter Filter]
   #if ( Warstwy )        	
        [0.765 color LevCol ]
     	 [0.770 color LevCol ]        	
      	 [0.770  color OrangeRed filter Filter]        
    #end         	
    [0.875 color Firebrick filter Filter]     //  very dark red  
    [0.990 color rgbf< 1.0, 0.0, 0.0, Filter>]
}; //color_map
                                  

/// #declare CartTexture
#declare CartTexture =
   texture {pigment { // create a smooth color gradation map
      gradient y
      color_map {Cartographic_Colors}  // default choice
      }
      finish { diffuse 0.75 phong 1 ambient .3}
      translate<0,-0.025,0>
   }; // CartTexture

/// #macro InsertColorMap( _ww, _LevCol) 
#macro InsertColorMap( _ww, _LevCol)
#debug "Inserting Cartographic_Colors....\n"
  pigment{
      gradient y
      color_map {
         [0.000 color NewMidnightBlue  filter Filter]
         [0.100 color Blue filter Filter]
         [0.148 color Cyan filter Filter]
         [0.245 color Green filter Filter]
        #if ( _ww  )
             [0.245 color _LevCol ]
            [0.250 color _LevCol ]
             [0.250 color Green filter Filter]  //  yellow lemon
        #end

        [0.400 color rgbf< 0.75, 1.0, 0.0, Filter>]     //  yellow lemon

        [0.495 color Yellow  filter Filter]
        #if ( _ww )
             [0.495 color _LevCol ]
             [0.500 color _LevCol ]
             [0.500 color Yellow filter Filter]
        #end

       [0.545 color Gold  filter Filter]
       [0.575 color Orange  filter Filter]

       [0.765 color OrangeRed  filter Filter]
       #if ( _ww )
            [0.765 color _LevCol ]
             [0.770 color _LevCol ]
             [0.770  color OrangeRed filter Filter]
        #end
        [0.875 color Firebrick filter Filter]     //  very dark red
        [0.990 color rgbf< 1.0, 0.0, 0.0, Filter>]
     }

 } //color_map
#end

// used by a.o. Potential.inc, VectorField.pov, ...
#declare _RGBMap2 = color_map { 
          [ 0.00 rgbt < 0.1, 0.1, .1, .90>]
          [ 0.15 rgbt < 0, 0, 1, .0>]
          [ 0.45 rgbt < 0, 1, 0, .0>]
          [ 0.75 rgbt < 1, 1, 0, .0>]
          [ 0.90 rgbt < 1, 0, 0, .0>]
          };     

#declare _RGBMap1 = color_map { 
          [ 0.05 rgbt < 1, 1, 1, .0>]
          [ 0.95 rgbt < 0, 0, 0, .0>]
          [ 0.95 rgbt < 1, 1, 1, .0>]
          };     

#macro SetRGBMap(_RGBm)
    #declare _RGBMap = _RGBm;
#end

#end // #ifndef(ColorMaps_INC)
