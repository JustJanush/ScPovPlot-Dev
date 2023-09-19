/****h* Histogram.inc/BarShapes.inc
* PURPOSE
*   Old. well known macro supplying Histogram.inc with customary shapes
*   There is a lot TODO: some automation and so on.
*   Changelog:
*   In macro McrEllipsoConeBasic(ecc, deg) 
*   Line: "rotate y*deg" changed[!] to "rotate -y*deg" to obtain intuitive rotation
*   |html <hr width=50% align="left">
*     *********************************************************
*     **   Tested on PovRay 3.7                              **
*     **   License: GNU GPL                                  **
*     **   Homepage:    http://scpovplot3d.sourceforge.net   **
*     *********************************************************
*     **   version: 3.1.0.5 (& have a nice time ;)             **
*     *********************************************************
* AUTHOR
*  Janusz Opi³a Ph.D.
*   jmo@agh.edu.pl, janusz.opila@gmail.com
*   Dept. of Applied Informatics
*   AGH University of Science & Technology, Cracow, Poland
*   Maintained by Janusz Opi³a Ph.D.
* COPYRIGHT
*  GNU GPL v.3 License
*  (c) 2012-now by Janusz Opi³a Ph.D.         
*  AGH University of Science and Technology
*
* HISTORY
*  2017-02-03, 3.1.0.5  - "PlusMinus" bars added
*  2017-10-15, 3.1.0.11 - HollowBar() corrections
*  2018-10-28, SetEmission#() macros added
****
  end of RoboDoc comment
****/

#ifndef  (BarShapes_INC)
#declare BarShapes_INC="2019-05-16";
#version 3.7;
/***************************/
#declare _FileName   = "BarShapes.inc";
#declare _FileVerMaj = "3.1.0.13";
#declare _FileVerMin = BarShapes_INC;

#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

#ifndef (Debug) // Just 4 safety
   #declare Debug=0;
#end

#include "shapes.inc"            
#include "CommonDefs.inc"
/********************************************/
#declare StandardBar = box{<-.5,0,.5>, <.5, 1, -.5>};
#declare CylinderBar = cylinder{0,<0,1,0>, .5};

#declare _Emission1 = .5;
#declare _Emission2 = .5;

#macro SetEmission1(_em)
    #declare _Emission1 = _em;
#end

#macro SetEmission2(_em)
    #declare _Emission2 = _em;
#end

#macro McrCylinderBar(ecc, deg) // ecc-entricity, deg-rotation in [deg]
   #declare obj = 
      cylinder{0,<0, .5, 0> .5               // black
      #if (ecc<1.0)
        scale <1, 2, ecc>
      #else
        scale <1/ecc, 2, 1>
      #end
      rotate -y*deg  // changed[!] to obtain more intuitive rotation def.
     #if (Photons)
        photons {    // photons block
           target 1
           refraction on
           reflection on
           collect off
        }
     #end         
     }
   
   object{obj}
#end

#declare CylinderQuadroBarUnion =
union {
  cylinder{0,<0,1,0>, .3 translate < .15, 0,  .15> }
  cylinder{0,<0,1,0>, .3 translate <-.15, 0, -.15> }
  cylinder{0,<0,1,0>, .3 translate <-.15, 0,  .15> }
  cylinder{0,<0,1,0>, .3 translate < .15, 0, -.15> }
}

#declare CylinderQuadroBar = object{ CylinderQuadroBarUnion};

#declare CylinderQuadroBarMerge =
union {

  cylinder{0,<0,1,0>, .3 translate < .15, 0,  .15> }
  cylinder{0,<0,1,0>, .3 translate <-.15, 0, -.15> }
  cylinder{0,<0,1,0>, .3 translate <-.15, 0,  .15> }
  cylinder{0,<0,1,0>, .3 translate < .15, 0, -.15> }

}


#declare EllipsoConeBasic = difference{
   sphere {0.0, .5}      // black
   plane {y,0}
}

#macro McrEllipsoConeBasic(ecc, deg) // ecc-entricity, deg-rotation in [deg]
   #declare obj =
   difference{
      sphere {0.0, .5}               // black
      plane {y,0}
      #if (ecc<1.0)
        scale <1, 2, ecc>
      #else
        scale <1/ecc, 2, 1>
      #end
      rotate -y*deg  // changed[!] to obtain intuitive rotation
     #if (Photons)
        photons {    // photons block
           target 1
           refraction on
           reflection on
           collect off
        }
     #end
   }
   object{obj}
#end


// fully defined bars
#declare EllipsoCone =
difference{
   sphere {              // make it reflective
       0.0, .5
       texture {
         pigment {
           color rgb <0.8,0.8,1.0>
         }
         finish{
           diffuse 0.3
           ambient 0.0
           specular 0.6
           reflection {
             0.8
             metallic
           }
           conserve_energy
         }
       }
   }
   plane {y,0}
   scale <1,2,1>
}

// handles setTexture, SetRGBFTColor_1, SetRGBFTColor_2, 
#macro HollowBar(YYY, ecc, deg, D) //  ecc-entricity, deg-rotation in [deg]
   #declare YYY=YScaleFactor*YYY;

  #if (YYY > MaxY)
    #declare MaxY=YYY;
    #declare DscMaxY=MaxY/YScaleFactor;
  #end              
  
    #if (DeclareLevel = 2) // risky solution!
        #local tmpmat1 = material{ texture{tmpTexture} interior { tmpInterior }}; 
    #elseif (DeclareLevel > 2)
        #local tmpmat1 = tmpMaterial;
    #else
        #local tmpmat1 = material{ texture { pigment{color tmpColour1 }
//                           normal { bumps 0.5 scale 0.05}
                          finish { diffuse .0 phong .4 emission 0.4 }
                } // end of texture
                interior { tmpInterior }
      }; // end of material -------------------
    #end

  
    #if (DeclareLevel >= 2) // risky solution!
      #local tmpmat2 = material{ texture{tmpTexture} interior { tmpInterior }};
    #elseif (DeclareLevel > 2)
        #local tmpmat2 = tmpMaterial;
    #else
      #local tmpmat2 = material{ texture { pigment{color tmpColour2 }
//                              normal { bumps 0.3 scale 0.05}
                          finish { diffuse 0.0 phong .4 emission 0.4
                                specular 0.6
                                roughness 0.005
                                reflection {
                                  0.1, 1.0
                                  fresnel on
                                }
                                conserve_energy
                          }
                } // end of texture     
                interior { tmpInterior }
      }; // end of material -------------------
    #end

   #declare obj =
   difference{
      box{
         <-Xside/2, 0, Zside/2>, <Xside/2, YYY, -Zside/2> material{tmpmat1}
      }
      #declare WT=D*(Xside+Zside)/2;
      box{
         <-Xside/2+WT, WT, Zside/2-WT>, <Xside/2-WT, YYY-WT, -Zside/2+WT> material{ tmpmat2}
      }

     #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect on
        }
     #end
      #if (ecc<1.0)
        scale <1, 1, ecc>
      #else
        scale <1/ecc, 1, 1>
      #end
     rotate y*deg

     }
   object{obj}
#end

/****************************************************/
// handles setTexture, SetRGBFTColor_1, SetRGBFTColor_2, 
#macro HollowBarOpen(YYY, ecc, deg, D) //  ecc-entricity, deg-rotation in [deg], walls width
   #declare YYY=YScaleFactor*YYY;

  #if (YYY > MaxY)
    #declare MaxY=YYY;
    #declare DscMaxY=MaxY/YScaleFactor;
  #end

  #local tmpmat1 = 
    #if (DeclareLevel = 2)
      material{ texture{tmpTexture}}
    #else
      material{ texture { pigment{color tmpColour1 }
//                           normal { bumps 0.5 scale 0.05}
                          finish { diffuse 0.0 phong .4 emission _Emission1 }
                } // end of texture
                interior { tmpInterior }
      } // end of material -------------------
    #end;

  #local tmpmat2 = 
    #if (DeclareLevel = 2)
      material{ texture{tmpTexture}}
    #else
      material{ texture { pigment{color tmpColour2 }
//                              normal { bumps 0.1 scale 0.02}
                          finish { diffuse .0 phong .4 emission _Emission2}
                } // end of texture
      } // end of material -------------------
    #end;

   #declare obj =
   difference{
      box{
         <-Xside/2, Tiny, Zside/2>, <Xside/2, YYY, -Zside/2> material{ tmpmat1 }
      }
      #declare WT=D*(Xside+Zside)/2;
      box{
         <-Xside/2+WT, WT, Zside/2-WT>, <Xside/2-WT, YYY+Tiny, -Zside/2+WT> material{ tmpmat2 }
      }

     cutaway_textures
     #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect on
        }
     #end
      #if (ecc<1.0)
        scale <1, 1, ecc>
      #else
        scale <1/ecc, 1, 1>
      #end
     rotate y*deg

     }
   object{obj}
#end

#macro HollowCylinder(YYY, ecc, deg, D) //  ecc-entricity, deg-rotation in [deg]
   #declare YYY=YScaleFactor*YYY;

  #if (YYY > MaxY)
    #declare MaxY=YYY;
    #declare DscMaxY=MaxY/YScaleFactor;
  #end

   #declare obj =
   difference{
      cylinder{
         <0, 0, 0>, <0, YYY, 0>, 0.5
          material{ texture { pigment{color tmpColour1 }
   //                           normal { bumps 0.5 scale 0.05}
                              finish { diffuse .0 phong .4 emission _Emission1 }
                    } // end of texture
                    interior { tmpInterior }

          } // end of material -------------------
      }
      #declare WT=D/2;
      cylinder{
         <0, WT, 0>, <0, YYY-WT, 0>, 0.5-WT
          material{ texture { pigment{color tmpColour2 }
//                              normal { bumps 0.3 scale 0.05}
                              finish { diffuse 0.0 phong .4 emission _Emission2
                                    specular 0.6
                                    roughness 0.005
                                    reflection {
                                      0.1, 1.0
                                      fresnel on
                                    }
                                    conserve_energy
                              }
                    } // end of texture
          } // end of material -------------------
      }

     #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect on
        }
     #end
      #if (ecc<1.0)
        scale <Xside, 1, Zside*ecc>
      #else
        scale <Xside/ecc, 1, Zside>
      #end
     rotate y*deg

     }
   object{obj}
#end

/****************************************************/
#macro HollowCylinderOpen(YYY, ecc, deg, D) //  ecc-entricity, deg-rotation in [deg]
   #declare YYY=YScaleFactor*YYY;

  #if (YYY > MaxY)
    #declare MaxY=YYY;
    #declare DscMaxY=MaxY/YScaleFactor;
  #end

   #declare obj =
   difference{
      cylinder{
         <0, Tiny, 0>, <0, YYY, 0>, 0.5
          material{ texture { pigment{color tmpColour1 }
   //                           normal { bumps 0.5 scale 0.05}
                              finish { diffuse 0.0 phong .4 emission _Emission1
                                    specular 0.6
                                    roughness 0.005
                                    reflection {
                                      0.1, 1.0
                                      fresnel on
                                    }
                                    conserve_energy
                              }

                    } // end of texture
                    interior { tmpInterior }

          } // end of material -------------------
      }
      #declare WT=D/2;
      cylinder{
         <0, WT, 0>, <0, YYY+Tiny, 0>, 0.5-WT
          material{ texture { pigment{color tmpColour2 }
//                              normal { bumps 0.1 scale 0.02}
                              finish { diffuse .0 phong .4 emission _Emission2 }
                    } // end of texture
          } // end of material -------------------
      }

     #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect on
        }
     #end
      #if (ecc<1.0)
        scale <Xside, 1, Zside*ecc>
      #else
        scale <Xside/ecc, 1, Zside>
      #end
     rotate y*deg

     }
   object{obj}
#end

#macro CylinderMQuadroBarUnion(R, D, Deg) // Radius, Separation, rotation [deg]
     CylinderMQuadroBar(R, D, Deg)       // just calls old macro
#end


#macro CylinderMQuadroBar(R, D, Deg) // Radius, Separation, rotation [deg]
#declare obj = union {
        cylinder{0,<0,1,0>, D }
        cylinder{0,<0,1,0>, R translate < D, 0,  0> }
        cylinder{0,<0,1,0>, R translate <-D, 0,  0> }
        cylinder{0,<0,1,0>, R translate < 0, 0,  D> }
        cylinder{0,<0,1,0>, R translate < 0, 0, -D> }

        rotate y*(Deg+45)

      #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect off
        }
      #end
   }
   object{obj}
#end


#macro CylinderMQuadroBarMerge(R, D, Deg) // Radius, Separation, rotation [deg]
#declare obj = merge {
        cylinder{0,<0,1,0>, D }
        cylinder{0,<0,1,0>, R translate < D, 0,  0> }
        cylinder{0,<0,1,0>, R translate <-D, 0,  0> }
        cylinder{0,<0,1,0>, R translate < 0, 0,  D> }
        cylinder{0,<0,1,0>, R translate < 0, 0, -D> }

        rotate y*(Deg+45)

      #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect off
        }
      #end
   }
   object{obj}
#end



#macro CylinderM2QuadroBarUnion(R, Deg) // Radius, rotation [deg], D - standard separation, fixed
    CylinderM2QuadroBar(R, Deg) // just calls old macro
#end

#macro CylinderM2QuadroBar(R, Deg) // Radius, rotation [deg], D - standard separation, fixed
#declare obj = union {
        #declare D=0.5-R;

        cylinder{0,<0,1,0>, R }
        cylinder{0,<0,1,0>, R translate < D, 0,  0> }
        cylinder{0,<0,1,0>, R translate <-D, 0,  0> }
        cylinder{0,<0,1,0>, R translate < 0, 0,  D> }
        cylinder{0,<0,1,0>, R translate < 0, 0, -D> }

        rotate y*(Deg+45)

      #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect off
        }
      #end
   }
   object{obj}
#end


#macro CylinderM2QuadroBarMerge(R, Deg) // Radius, rotation [deg], D - standard separation, fixed
#declare obj = merge {
        #declare D=0.5-R;

        cylinder{0,<0,1,0>, R }
        cylinder{0,<0,1,0>, R translate < D, 0,  0> }
        cylinder{0,<0,1,0>, R translate <-D, 0,  0> }
        cylinder{0,<0,1,0>, R translate < 0, 0,  D> }
        cylinder{0,<0,1,0>, R translate < 0, 0, -D> }

        rotate y*(Deg+45)

      #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect off
        }
      #end
   }
   object{obj}
#end
// YYY-value, ecc-entricity, deg-rotation in [deg], walls width, photo name, y/x image ratio, text (short!!), text texture
// handles setTexture, SetRGBFTColor_1, SetRGBFTColor_2, 
#macro PhotoBar(YYY, ecc, deg, D, _Img, _yxratio, _Txt,_txt_txtr) 
   #declare YYY=YScaleFactor*YYY;

  #if (YYY > MaxY)
    #declare MaxY=YYY;
    #declare DscMaxY=MaxY/YScaleFactor;
  #end
  #declare obj = merge {
  #local _YSL = 1.15*YScaleFactor*sqrt(Xside*Zside);                  
//  light_source { <  0, _YSL+YYY ,        0> color rgb .6}
//  light_source { <  0, _YSL+YYY ,        0> color rgb .6 looks_like{sphere{ 0 .5 texture{pigment{color rgbt<0,0,1,.5>} finish{emission 1}}}}} // natural light source, white  // prev: < -Xside/3, 20,  Zside/2>
    
//  light_source { <  0, 0.3*YYY, -3*Zside> color rgb .1 shadowless}
//  light_source { <  0, 0.3*YYY, -3*Zside> color rgb .1 looks_like{sphere{ 0 .5 texture{pigment{color rgbt<0,1,0,.5>} finish{emission 1}}}}} // natural light source, white  // prev: <  Xside/3, 10, -3*Zside>
    
    light_source { <  0, 0.5*_YSL, -5*Zside> color rgb .3 shadowless}
//light_source { <  0, 0.5*_YSL, -5*Zside> looks_like{sphere{ 0 .5 texture{pigment{color rgbt<1,0,0,.5>} finish{emission 1}}}}} // natural light source, white  //  New - labels enlightement

// now insert photo     
      #if (strlen(_Img)>0) // picture given
         polygon {
           5,
              <0, 0>, <0, 1>, <1, 1>, <1, 0>, <0, 0>
           // experimental feature!!
           texture {
               pigment{color rgb .5}
               finish{ emission .4}
           }
           #if ( __MattSet ) texture { __MattTexture } #end 
           
           texture {
             finish { ImgFinish }
             pigment {
                     #if (file_exists(_Img))
                       ImageType( _Img )
                       #if (Debug)  
                         #debug concat(endl, input_file_name, ": [ext=",__ImageType, "]",endl)  
                       #end   
                       image_map { // exr | gif | hdr | iff | jpeg | pgm | png | ppm | sys | tga | tiff
                          #if (__ImageType="jpeg") jpeg #end     
                          #if (__ImageType="tiff") tiff #end     
                          #if (__ImageType= "png")  png #end     
                          #if (__ImageType= "gif")  gif #end     
                          #if (__ImageType= "iff")  iff #end     
                          #if (__ImageType= "hdr")  hdr #end     
                          #if (__ImageType= "exr")  exr #end     
                          #if (__ImageType= "tga")  tga #end     
                          #if (__ImageType= "pgm")  pgm #end     
                          #if (__ImageType= "ppm")  ppm #end     
                          #if (__ImageType= "sys")  sys #end     
                          _Img    // actual file name
                          gamma #if (ImgGamma>0) ImgGamma #else srgb #end 
                          #if (!TilXYimage) once #end
                          #switch (ImgInter)
                             #case (2) interpolate 2 #break // bilinear
                             #case (3) interpolate 3 #break // bicubic
                             #case (4) interpolate 4 #break // normalized
                             #else #debug "image_map: no interpolation until enter 2, 3 or 4" #break
                          #end
                          #if ( __ImgFilterSet   ) filter     all __ImgFilterPower    #end            
                          #if ( __ImgTransmitSet ) transmit   all __ImgTransmitPower  #end            
                        }
                    #else
                        color rgb 0.5
                        #debug concat("\nImage specified not found:\"",_Img,"\"\n")
                    #end
                }
             }

             scale <.7*Xside, _yxratio*.7*Xside, 1>
             translate < -Xside/2, 0, -Zside/2-.01 >
           }
      #end
// end insert photo                 

//==================================
  #local tmpmat1 = 
    #if (DeclareLevel = 2)
      material{ texture{tmpTexture}};
    #else
      material{ texture { pigment{color rgbft tmpColour1 }
//                       normal { bumps 0.5 scale 0.05}
//                       finish { scDullMirror diffuse .30 phong .4 emission .3 }
                       finish { Dull diffuse .30 phong .4 emission .3 }
                  } // end of texture
                interior { tmpInterior }
      }; // end of material -------------------
    #end

  #local tmpmat2 = 
    #if (DeclareLevel = 2)
      material{ texture{tmpTexture}};
    #else
      material{ texture { pigment{color rgbft tmpColour2 }
//                              normal { bumps 0.3 scale 0.05}
                          finish { diffuse 0.0 phong .4
                                emission .4
                                specular 0.6
                                roughness 0.005
                                reflection {
                                  0.1, 1.0
                                  fresnel on
                                }
                                conserve_energy
                          }
                } // end of texture
      }; // end of material -------------------
    #end
//==================================

   #if (D>0)
   difference{
      box{
         <-Xside/2, 0, Zside/2>, <Xside/2, YYY, -Zside/2>
         material{tmpmat1}
      }
      #declare WT=D*(Xside+Zside)/2;
      box{
         <-Xside/2+WT, WT, Zside/2-WT>, <Xside/2-WT, YYY-WT, -Zside/2+WT> material{tmpmat2}
      }
   } 

   #elseif (D<0)
       #declare D=abs(D);
       difference{
          box{
             <-Xside/2, 0, Zside/2>, <Xside/2, YYY, -Zside/2>
             material{tmpmat1}          
          }
          #declare WT=D*(Xside+Zside)/2;
          box{
             <-Xside/2+WT, WT, Zside/2-WT>, <Xside/2-WT, 2*YYY-WT, -Zside/2+WT>
             material{tmpmat2}
          }
       } 
     #else // ... D<=0 full bar
      box{
         <-Xside/2, 0, Zside/2>, <Xside/2, YYY, -Zside/2> material{tmpmat1}
      }
     
    #end // ...if D>0
    
    #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect on
        }
    #end
    #if (ecc<1.0)
       scale <1, 1, ecc>
    #else
       scale <1/ecc, 1, 1>
    #end
    rotate y*deg
     

// insert description (keep it short!!)
    #declare txtobj = object{
       Text(_Txt)
       texture{_txt_txtr}
    };   
    #declare maxtxt=max_extent(txtobj);
//    #debug concat("maxtxt= ",str((Xside/2-maxtxt.x)/2,5,3), "\n" )   

    
// end description object is ready!

}; 
    union{
    object{
       txtobj
       scale < .3*Xside/maxtxt.x, .3*Xside, LettrDepth>
       translate <Xside/10, 0.1, -Zside/2-.1>
    }
    object{obj}
    }
#end // PhotoBar

// YYY-value, ecc-entricity, deg-rotation in [deg], walls width, photo name, y/x image ratio, text (short!!), text texture
// handles setTexture, SetRGBFTColor_1, SetRGBFTColor_2, 
#macro PhotoBarUp(YYY, ecc, deg, D, _Img, _yxratio, _Txt,_txt_txtr) 
   #declare YYY=YScaleFactor*YYY;

  #if (YYY > MaxY)
    #declare MaxY=YYY;
    #declare DscMaxY=MaxY/YScaleFactor;
  #end
  #declare obj = merge {
  #local _YSL = 1.15*YScaleFactor*sqrt(Xside*Zside);                  
//  light_source { <  0, _YSL+YYY ,        0> color rgb .6}
//  light_source { <  0, _YSL+YYY ,        0> color rgb .6 looks_like{sphere{ 0 .5 texture{pigment{color rgbt<0,0,1,.5>} finish{emission 1}}}}} // natural light source, white  // prev: < -Xside/3, 20,  Zside/2>
    
//  light_source { <  0, 0.3*YYY, -3*Zside> color rgb .1 shadowless}
//  light_source { <  0, 0.3*YYY, -3*Zside> color rgb .1 looks_like{sphere{ 0 .5 texture{pigment{color rgbt<0,1,0,.5>} finish{emission 1}}}}} // natural light source, white  // prev: <  Xside/3, 10, -3*Zside>
    
    light_source { <  0, 0.5*_YSL, -5*Zside> color rgb .3 shadowless}
//light_source { <  0, 0.5*_YSL, -5*Zside> looks_like{sphere{ 0 .5 texture{pigment{color rgbt<1,0,0,.5>} finish{emission 1}}}}} // natural light source, white  //  New - labels enlightement

// now insert photo     
      #if (strlen(_Img)>0) // picture given
         polygon {
           5,
              <0, 0>, <0, 1>, <1, 1>, <1, 0>, <0, 0>
           // experimental feature!!
           texture {
               pigment{color rgb .5}
               finish{ emission .4}
           }
           #if ( __MattSet ) texture { __MattTexture } #end 
           
           texture {
             finish { ImgFinish }
             pigment {
                     #if (file_exists(_Img))
                       ImageType( _Img )
                       #if (Debug)  
                         #debug concat(endl, input_file_name, ": [ext=",__ImageType, "]",endl)  
                       #end   
                       image_map { // exr | gif | hdr | iff | jpeg | pgm | png | ppm | sys | tga | tiff
                          #if (__ImageType="jpeg") jpeg #end     
                          #if (__ImageType="tiff") tiff #end     
                          #if (__ImageType= "png")  png #end     
                          #if (__ImageType= "gif")  gif #end     
                          #if (__ImageType= "iff")  iff #end     
                          #if (__ImageType= "hdr")  hdr #end     
                          #if (__ImageType= "exr")  exr #end     
                          #if (__ImageType= "tga")  tga #end     
                          #if (__ImageType= "pgm")  pgm #end     
                          #if (__ImageType= "ppm")  ppm #end     
                          #if (__ImageType= "sys")  sys #end     
                          _Img    // actual file name
                          gamma #if (ImgGamma>0) ImgGamma #else srgb #end 
                          #if (!TilXYimage) once #end
                          #switch (ImgInter)
                             #case (2) interpolate 2 #break // bilinear
                             #case (3) interpolate 3 #break // bicubic
                             #case (4) interpolate 4 #break // normalized
                             #else #debug "image_map: no interpolation until enter 2, 3 or 4" #break
                          #end
                          #if ( __ImgFilterSet   ) filter     all __ImgFilterPower    #end            
                          #if ( __ImgTransmitSet ) transmit   all __ImgTransmitPower  #end            
                        }
                    #else
                        color rgb 0.5
                        #debug concat("\nImage specified not found:\"",_Img,"\"\n")
                    #end
                }
             }
             scale <Xside, _yxratio*Xside, 1>
             translate < -Xside/2, YYY+Tiny, (1-Tiny)*Zside/2 >
           }
      #end
// end insert photo                 

//==================================
  #local tmpmat1 = 
    #if (DeclareLevel = 2)
      material{ texture{tmpTexture}};
    #else
      material{ texture { pigment{color rgbft tmpColour1 }
//                       normal { bumps 0.5 scale 0.05}
//                       finish { scDullMirror diffuse .30 phong .4 emission .3 }
                       finish { Dull diffuse .30 phong .4 emission .3 }
                  } // end of texture
                interior { tmpInterior }
      }; // end of material -------------------
    #end

  #local tmpmat2 = 
    #if (DeclareLevel = 2)
      material{ texture{tmpTexture}};
    #else
      material{ texture { pigment{color rgbft tmpColour2 }
//                              normal { bumps 0.3 scale 0.05}
                          finish { diffuse 0.0 phong .4
                                emission .4
                                specular 0.6
                                roughness 0.005
                                reflection {
                                  0.1, 1.0
                                  fresnel on
                                }
                                conserve_energy
                          }
                } // end of texture
      }; // end of material -------------------
    #end
//==================================

   #if (D>0)
   difference{
      box{
         <-Xside/2, 0, Zside/2>, <Xside/2, YYY, -Zside/2>
         material{tmpmat1}
      }
      #declare WT=D*(Xside+Zside)/2;
      box{
         <-Xside/2+WT, WT, Zside/2-WT>, <Xside/2-WT, YYY-WT, -Zside/2+WT> material{tmpmat2}
      }
   } 

   #elseif (D<0)
       #declare D=abs(D);
       difference{
          box{
             <-Xside/2, 0, Zside/2>, <Xside/2, YYY, -Zside/2>
             material{tmpmat1}          
          }
          #declare WT=D*(Xside+Zside)/2;
          box{
             <-Xside/2+WT, WT, Zside/2-WT>, <Xside/2-WT, 2*YYY-WT, -Zside/2+WT>
             material{tmpmat2}
          }
       } 
     #else // ... D<=0 full bar
      box{
         <-Xside/2, 0, Zside/2>, <Xside/2, YYY, -Zside/2> material{tmpmat1}
      }
     
    #end // ...if D>0
    
    #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect on
        }
    #end
    #if (ecc<1.0)
       scale <1, 1, ecc>
    #else
       scale <1/ecc, 1, 1>
    #end
    rotate y*deg
     

// insert description (keep it short!!)
    #declare txtobj = object{
       Text(_Txt)
       texture{_txt_txtr}
    };   
    #declare maxtxt=max_extent(txtobj);
//    #debug concat("maxtxt= ",str((Xside/2-maxtxt.x)/2,5,3), "\n" )   

    
// end description object is ready!

}; 
    union{
    object{
       txtobj
       scale < .3*Xside/maxtxt.x, .3*Xside, LettrDepth>
       translate <Xside/10, 0.1, -Zside/2-.1>
    }
    object{obj}
    }
#end // PhotoBar

// YYY-value, ecc-entricity, deg-rotation in [deg], walls width, square photo name, text (short!!), text texture
// _Gadget is assumed as 1x1x# box, aligned with Xp/Yp plane (vertical!).
// added 2015-12-07
// handles setTexture, SetRGBFTColor_1, SetRGBFTColor_2, 
#macro GadgetBar(YYY, ecc, deg, D, _Gadget, _Txt,_txt_txtr) // _Gadget is gadget to add to bar. X-Y plane, 1x1 square, back plane is on poray X-Y plane 
   #declare YYY=YScaleFactor*YYY;

  #if (YYY > MaxY)
    #declare MaxY=YYY;
    #declare DscMaxY=MaxY/YScaleFactor;
  #end
  #declare obj = merge {
    #local _YSL = 1.15*YScaleFactor*sqrt(Xside*Zside);                  
  light_source { <  0, _YSL+YYY ,        0> color rgb .3 shadowless}
//  light_source { <  0, _YSL+YYY ,        0> looks_like{sphere{ 0 .5 texture{pigment{color rgbt<0,0,1,.5>} finish{emission 1}}}}} // natural light source, white  // prev: < -Xside/3, 20,  Zside/2>
    
    light_source { <  0, 0.5*YYY, -3*Zside> color rgb .02 shadowless}
//  light_source { <  0, 0.5*YYY, -3*Zside> looks_like{sphere{ 0 .5 texture{pigment{color rgbt<0,1,0,.5>} finish{emission 1}}}}} // natural light source, white  // prev: <  Xside/3, 10, -3*Zside>
    
//    light_source { <  0, 1.5*_YSL, -5*Zside> color rgb .2 shadowless}
//  light_source { <  0, 1.5*_YSL, -5*Zside> looks_like{sphere{ 0 .5 texture{pigment{color rgbt<1,0,0,.5>} finish{emission 1}}}}} // natural light source, white  //  New - labels enlightement

// now insert photo     
    object{
        _Gadget
         scale <.7*Xside, .7*Xside, .7*Xside>
         translate < -Xside/2, 0, -Zside/2-.01 >
   }
// end insert gadget                 

//==================================
  #local tmpmat1 = 
    #if (DeclareLevel = 2)
      material{ texture{tmpTexture}};
    #else
      material{ texture { pigment{color rgbft tmpColour1 }
                       //   normal { bumps 0.5 scale 0.05}
                       finish { scDullMirror diffuse .30 phong .6 emission .6 }
                       // finish { scDullMirror diffuse .030 phong .16 emission .8 }
                       // finish {Mirror}
                } // end of texture
                interior { tmpInterior }
      }; // end of material -------------------
    #end

  #local tmpmat2 = 
    #if (DeclareLevel = 2)
      material{ texture{tmpTexture}};
    #else
      material{ texture { pigment{color rgbft tmpColour2 }
//                              normal { bumps 0.3 scale 0.05}
                          finish { diffuse 0.0 phong .4
                                emission .6
                                specular 0.6
                                roughness 0.005
                                reflection {
                                  0.1, 1.0
                                  fresnel on
                                }
                                conserve_energy
                          }
                } // end of texture
      }; // end of material -------------------
    #end
//==================================
          
   #if (D>0)
       difference{
          box{
             <-Xside/2, Tiny, Zside/2>, <Xside/2, YYY, -Zside/2> material{ tmpmat1 }
          }
          #declare WT=D*(Xside+Zside)/2;
          box{
             <-Xside/2+WT, WT, Zside/2-WT>, <Xside/2-WT, YYY-WT, -Zside/2+WT> material{ tmpmat2 } // end of material -------------------
          }
       } 
     #elseif (D<0) // ... D=0 full bar, D<0 - open bar
       #declare D = abs(D);
       difference{
          box{
             <-Xside/2, Tiny, Zside/2>, <Xside/2, YYY, -Zside/2> material{ tmpmat1 }
          }
          #declare WT=D*(Xside+Zside)/2;
          box{
             <-Xside/2+WT, WT, Zside/2-WT>, <Xside/2-WT, 2*YYY, -Zside/2+WT>
              material{ tmpmat2 }
          }
       } 
     #else 
          box{
             <-Xside/2, Tiny, Zside/2>, <Xside/2, YYY, -Zside/2>  material{ tmpmat1 }
          }
    #end // ...if D>0
    
    #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect on
        }
    #end
    #if (ecc<1.0)
       scale <1, 1, ecc>
    #else
       scale <1/ecc, 1, 1>
    #end
}; 
// insert description (keep it short!!)
#declare txtobj = object{
   Text(_Txt)
   texture{_txt_txtr}
};   
#declare maxtxt=max_extent(txtobj);
//    #debug concat("maxtxt= ",str((Xside/2-maxtxt.x)/2,5,3), "\n" )   

    
// end description object is ready!


    union{
        object{
           txtobj
           scale < .26*Xside/maxtxt.x, .26*Xside/maxtxt.x, LettrDepth>
           translate <.22*Xside, 0.02*Xside, -Zside/2-.1>
        }
        object{obj}
        rotate -y*deg

    }
#end


//========================
// Glassy Photo Frame
#macro GlassyPhotoFrame( _YYY, _Img, _D, _P, _T1, _T2 )     // Done !                         
    #ifdef (Debug)
    //  #debug concat("maxX= ", str(SceneMaxX, 10, 6),"\n")
    
    
    #end

union{  // add image
      #if (strlen(_Img)>0)
         polygon {
           5,
           #if (TilXYimage)
              <0, 0>, <0, _YYY>, <1, _YYY>, <1, 0>, <0, 0>
           #else
              <0, 0>, <0, 1>, <1, 1>, <1, 0>, <0, 0>
           #end                     
           #if ( __MattSet )  texture{__MattTexture}  #end 
           texture {            
             pigment {
                     #if (file_exists(_Img))
                       ImageType( _Img )
                       #if (Debug)  
                         #debug concat(endl, input_file_name, ": [ext=",__ImageType, "]",endl)  
                       #end   
                       image_map { // exr | gif | hdr | iff | jpeg | pgm | png | ppm | sys | tga | tiff
                          #if (__ImageType="jpeg") jpeg #end     
                          #if (__ImageType="tiff") tiff #end     
                          #if (__ImageType= "png")  png #end     
                          #if (__ImageType= "gif")  gif #end     
                          #if (__ImageType= "iff")  iff #end     
                          #if (__ImageType= "hdr")  hdr #end     
                          #if (__ImageType= "exr")  exr #end     
                          #if (__ImageType= "tga")  tga #end     
                          #if (__ImageType= "pgm")  pgm #end     
                          #if (__ImageType= "ppm")  ppm #end     
                          #if (__ImageType= "sys")  sys #end     
                          _Img    // actual file name
                          gamma #if (ImgGamma>0) ImgGamma #else srgb #end 
                          #if (!TilXYimage) once #end
                          #switch (ImgInter)
                             #case (2) interpolate 2 #break // bilinear
                             #case (3) interpolate 3 #break // bicubic
                             #case (4) interpolate 4 #break // normalized
                             #else #debug "image_map: no interpolation until enter 2, 3 or 4" #break
                          #end
                          #if ( __ImgFilterSet   ) filter     all __ImgFilterPower    #end            
                          #if ( __ImgTransmitSet ) transmit   all __ImgTransmitPower  #end            
                        }
                    #else
                        color rgb 0.5
                        #debug concat("\nImage specified not found",_Img,"\n")
                    #end
                }
                finish { ImgFinish }
             }

             #if (!TilXYimage) scale <1, _YYY, 1> #end
             translate <0, 0, -_D*(1-_P)>
           }
///*

     #if ( _P<=0 )
          box { 0, <1, _YYY, -_D+Tiny>  // 
                material{ texture{ _T1 }}      // end of texture
          }      
     #else
          box { 0, <1, _YYY, -_D*_P+10*Tiny>  // 
                material{ texture{ _T1 } }      // end of texture
                translate <0,0,-_D*(1-_P)-10*Tiny>
          }      
      
          box { 0, <1, _YYY, -_D*(1-_P)+10*Tiny>  // 
                material{ texture{ _T2 } }      // end of texture
          }      
      
     #end
      
     #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect on
        }
     #end
  #end  // No picture

}
#end // XY

//=====================================
//==[ move to BarShapes.inc ]==========
//=====================================                                            
// YYY-value, ecc-entricity, deg-rotation in [deg], walls width, photo name, y/x image ratio, text (short!!), text texture
#macro PlusMinusBar(YYY, YYY0, ecc, deg, D, _Img, _yxratio, _Txt,_txt_txtr) 
   #declare YYY = YScaleFactor*YYY; // present value
   #declare YYY0= YScaleFactor*YYY0;// previous value

  #if (YYY > MaxY)
    #declare MaxY=YYY;
    #declare DscMaxY=MaxY/YScaleFactor;
  #end
  #if (YYY0 > MaxY)
    #declare MaxY=YYY0;
    #declare DscMaxY=MaxY/YScaleFactor;
  #end

  #declare obj = merge {

    light_source { <  0, 1.5*YYY,        0> color rgb .1 } // natural light source, white  // prev: < -Xside/3, 20,  Zside/2>
//    light_source { <  0, 0.5*YYY, -3*Zside> color rgb .2471 } // natural light source, white  // prev: <  Xside/3, 10, -3*Zside>
    light_source { <  0,     0.0, -5*Zside> color rgb .1 } // natural light source, white  //  New - labels enlightement

// now insert photo     
      #if (strlen(_Img)>0) // picture given
         polygon {
           5,
              <0, 0>, <0, 1>, <1, 1>, <1, 0>, <0, 0>
           // experimental feature!!
           texture {
               pigment{color rgb .5}
               finish{ emission 1}
           }
           #if ( __MattSet ) texture { __MattTexture } #end 
           
           texture {
             finish { ImgFinish }
             pigment {
                     #if (file_exists(_Img))
                       ImageType( _Img )
                       #if (Debug)  
                         #debug concat(endl, input_file_name, ": [ext=",__ImageType, "]",endl)  
                       #end   
                       image_map { // exr | gif | hdr | iff | jpeg | pgm | png | ppm | sys | tga | tiff
                          #if (__ImageType="jpeg") jpeg #end     
                          #if (__ImageType="tiff") tiff #end     
                          #if (__ImageType= "png")  png #end     
                          #if (__ImageType= "gif")  gif #end     
                          #if (__ImageType= "iff")  iff #end     
                          #if (__ImageType= "hdr")  hdr #end     
                          #if (__ImageType= "exr")  exr #end     
                          #if (__ImageType= "tga")  tga #end     
                          #if (__ImageType= "pgm")  pgm #end     
                          #if (__ImageType= "ppm")  ppm #end     
                          #if (__ImageType= "sys")  sys #end     
                          _Img    // actual file name
                          gamma #if (ImgGamma>0) ImgGamma #else srgb #end 
                          #if (!TilXYimage) once #end
                          #switch (ImgInter)
                             #case (2) interpolate 2 #break // bilinear
                             #case (3) interpolate 3 #break // bicubic
                             #case (4) interpolate 4 #break // normalized
                             #else #debug "image_map: no interpolation until enter 2, 3 or 4" #break
                          #end
                          #if ( __ImgFilterSet   ) filter     all __ImgFilterPower    #end            
                          #if ( __ImgTransmitSet ) transmit   all __ImgTransmitPower  #end            
                        }
                    #else
                        color rgb 0.5
                        #debug concat("\nImage specified not found",_Img,"\n")
                    #end
                }
             }
             scale <.7*Xside, _yxratio*.7*Xside, 1>
             translate < -Xside/2, 0, -Zside/2-.01 >
           }
      #end
// end insert photo                 
    
   #if (D>0)
   difference{
      box{
         <-Xside/2, 0, Zside/2>, <Xside/2, YYY, -Zside/2>
          material{ texture { pigment{color rgbft tmpColour1 }
   //                           normal { bumps 0.5 scale 0.05}
                           finish { scDullMirror } // diffuse .0 phong .4 ambient .5 }
                    } // end of texture
                    interior { tmpInterior }
          } // end of material -------------------
      
      #declare WT=D*(Xside+Zside)/2;
      box{
         <-Xside/2+WT, WT, Zside/2-WT>, <Xside/2-WT, YYY-WT, -Zside/2+WT>
          material{ texture { pigment{color rgbft tmpColour2 }
//                              normal { bumps 0.3 scale 0.05}
                              finish { diffuse 0.0 phong .4
                                    specular 0.6
                                    roughness 0.005
                                    reflection {
                                      0.1, 1.0
                                      fresnel on
                                    }
                                    conserve_energy
                              }
                    } // end of texture
          } // end of material -------------------
        }
     } 
     #else // ... D<=0 full bar
       box{// 1: base
          <-Xside/2,   0,  Zside/2>, 
          < Xside/2, YYY, -Zside/2>
          material{ texture { pigment{color rgbft tmpColour1 }
   //                           normal { bumps 0.5 scale 0.05}
                              finish { scDullMirror emission 0.6} // diffuse .0 phong .4 ambient .5 
                     }
                     interior { tmpInterior }
          } // end of material
       } // box
       #if (YYY-YYY0>Tiny)
          box{// 2: delta
             <-Xside/2, YYY0-_MarkerHeight, Zside/2>, <Xside/2, YYY0, -Zside/2>
              scale<_MarkerScale1, 1, _MarkerScale1>
              material{ texture { pigment{color rgbft <0,1,0,0,0> }
       //                           normal { bumps 0.5 scale 0.05}
                                  finish { scDullMirror emission .6} // diffuse .0 phong .4 ambient .5 
                        }
                        interior { tmpInterior }
              } // end of material
          }
       #elseif (YYY0-YYY>Tiny)   
          box{// 2: delta
             <-Xside/2, YYY0-_MarkerHeight, Zside/2>, <Xside/2, YYY0, -Zside/2>
              material{ texture { pigment{color rgbft <1,0,0,0,0.5> }
       //                           normal { bumps 0.5 scale 0.05}
                                  finish { scDullMirror emission .6} // diffuse .0 phong .4 ambient .5 
                        }
                        interior { tmpInterior }
              } // end of material        
              scale<_MarkerScale2, 1, _MarkerScale2>
          }
       #else
          box{// 2: delta
              < -Xside/2,          YYY+Tiny,  Zside/2>, 
              <  Xside/2, YYY+_MarkerHeight, -Zside/2>
              scale<_MarkerScale2, 1, _MarkerScale2>
              material{ texture { pigment{color rgbft <.3,.3,.3,0,0.5> }
       //                           normal { bumps 0.5 scale 0.05}
                                  finish { scDullMirror emission .7} // diffuse .0 phong .4 ambient .5 
                        }
                        interior { tmpInterior }
              } // end of material        
          }
       #end
    #end // ...if D>0
//    }   
    
    #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect on
        }
    #end
    #if (ecc<1.0)
       scale <1, 1, ecc>
    #else
       scale <1/ecc, 1, 1>
    #end
    rotate y*deg
     

// insert description (keep it short!!)
    #declare txtobj = object{
       Text(_Txt)
       texture{_txt_txtr}
    };   
    #declare maxtxt=max_extent(txtobj);
//    #debug concat("maxtxt= ",str((Xside/2-maxtxt.x)/2,5,3), "\n" )   

    
// end description object is ready!

}; 
    union{
    object{
       txtobj
       scale < .3*Xside/maxtxt.x, .3*Xside, LettrDepth>
       translate <Xside/10, 0.1, -Zside/2-.1>
    }
    object{obj}
    }
#end

//==================================
#declare _MarkerHeight = 0.1;
#macro SetMarkerHeight(_SM)
 #declare _MarkerHeight = _SM;
#end          

#declare _MarkerScale1=0.5;
#macro SetMarkerScaleUp(_SC)
 #declare _MarkerScale1 = _SC;
#end          

#declare _MarkerScale2=0.5;
#macro SetMarkerScaleDn(_SC)
 #declare _MarkerScale2 = _SC;
#end          
                                                
#declare _tstobj =  cylinder{
  <0,Tiny,0>, <0,1,0>, 0.5
};

#declare _tstbox =  box{
  <-0.5,Tiny,0.5>, <0.5,1,-0.5>
};

                                                
// YYY-value, ecc-entricity, deg-rotation in [deg], walls width, square photo name, text (short!!), text texture
// _Gadget is assumed as 1x1x# box, aligned with Xp/Yp plane (vertical!).
// added 2015-12-07
#macro GadgetBarPM(YYY, YYY0, ecc, deg, D, _obj, _Gadget, _Txt,_txt_txtr) // _Gadget is gadget to add to bar. X-Y plane, 1x1 square, back plane is on poray X-Y plane 
   #declare YYY  = YScaleFactor*YYY;
   #declare YYY0 = YScaleFactor*YYY0;

  #if (YYY > MaxY)
    #declare MaxY=YYY;
    #declare DscMaxY=MaxY/YScaleFactor;
  #end                  
  #if (YYY0 > MaxY)
    #declare MaxY=YYY0;
    #declare DscMaxY=MaxY/YScaleFactor;
  #end

  #declare obj = merge {

    light_source { <  0, 1.5*YYY,        0> color rgb .471 } // natural light source, white  // prev: < -Xside/3, 20,  Zside/2>
    light_source { <  0, 0.5*YYY, -3*Zside> color rgb .12471 } // natural light source, white  // prev: <  Xside/3, 10, -3*Zside>
    light_source { <  0,     0.0, -5*Zside> color rgb .1 } // natural light source, white  //  New - labels enlightement

// now insert gadget     
    object{
        _Gadget
         scale <.7*Xside, .7*Xside, .7*Xside>
         translate < -Xside/2, 0, -Zside/2-.01 >
   }
// end insert gadget                 
  #if (D>0)
   difference{
      box{
         <-Xside/2, Tiny, Zside/2>, <Xside/2, YYY, -Zside/2>
          material{ texture { pigment{color rgbft tmpColour1 }
   //                           normal { bumps 0.5 scale 0.05}
                              finish { scDullMirror } // diffuse .0 phong .4 ambient .5 }
                    } // end of texture
                    interior { tmpInterior }

          } // end of material -------------------
      }
      #declare WT=D*(Xside+Zside)/2;
      box{
         <-Xside/2+WT, WT, Zside/2-WT>, <Xside/2-WT, YYY-WT, -Zside/2+WT>
          material{ texture { pigment{color rgbft tmpColour2 }
//                              normal { bumps 0.3 scale 0.05}
                              finish { diffuse 0.0 phong .4
                                    specular 0.6
                                    roughness 0.005
                                    reflection {
                                      0.1, 1.0
                                      fresnel on
                                    }
                                    conserve_energy
                              }
                    } // end of texture
          } // end of material -------------------
        }
     } 
     #else // ... D<=0 full bar
       object{ 
         _obj
          scale<Xside,YYY,Zside>
          material{ texture { pigment{color rgbft tmpColour1} 
//                           normal { bumps 0.5 scale 0.05}
                         finish { scDullMirror emission 0.2 } // diffuse .0 phong .4 ambient .5 }
                    } // end of texture
                    interior { tmpInterior }
          } // end of material -------------------
       }
       #if (YYY-YYY0>Tiny)      
           object{ 
             _obj
              scale<_MarkerScale1*Xside,_MarkerHeight,_MarkerScale1*Zside>
              translate <0,YYY0,0>
              material{ texture { pigment{color rgbft <0,1,0,0,0>} 
    //                           normal { bumps 0.5 scale 0.05}
                             finish { scDullMirror emission 0.6 } // diffuse .0 phong .4 ambient .5 }
                        } // end of texture
                        interior { tmpInterior }
              } // end of material -------------------
           }
       #elseif(YYY0-YYY>Tiny)   
           object{ 
             _obj
              scale<_MarkerScale2*Xside,_MarkerHeight,_MarkerScale2*Zside> // reduced size...
              translate <0,YYY0,0>
              material{ texture { pigment{color rgbft <1,0,0,0,0>} 
    //                           normal { bumps 0.5 scale 0.05}
                             finish { scDullMirror emission 0.6 } // diffuse .0 phong .4 ambient .5 }
                        } // end of texture
                        interior { tmpInterior }
              } // end of material -------------------
           }
       #else
           object{ 
             _obj
              scale<_MarkerScale2*Xside, _MarkerHeight, _MarkerScale2*Zside>
              translate <0,YYY+Tiny,0>
              material{ texture { pigment{color rgbft <0.3,.3,.3,0,0>} 
    //                           normal { bumps 0.5 scale 0.05}
                             finish { scDullMirror emission 0.6 } // diffuse .0 phong .4 ambient .5 }
                        } // end of texture
                        interior { tmpInterior }
              } // end of material -------------------
           }
       #end
    #end // ...if D>0
    
    #if (Photons)
        photons {  // photons block
           target 1
           refraction on
           reflection on
           collect on
        }
    #end
    #if (ecc<1.0)
       scale <1, 1, ecc>
    #else
       scale <1/ecc, 1, 1>
    #end
}; 
// insert description (keep it short!!)
#declare txtobj = object{
   Text(_Txt)
   texture{_txt_txtr}
};   
#declare maxtxt=max_extent(txtobj);
//    #debug concat("maxtxt= ",str((Xside/2-maxtxt.x)/2,5,3), "\n" )   

// end description object is ready!
    union{
        object{
           txtobj
           scale < .26*Xside/maxtxt.x, .26*Xside/maxtxt.x, LettrDepth>
           translate <.22*Xside, 0.02*Xside, -Zside/2-.1>
        }
        object{obj}
        rotate -y*deg
    }
#end
//=====================================
//==] move to BarShapes.inc [==========
//=====================================                                            
       
#end  //#ifndef (BarShapes_INC)


/*
prism {
    linear_sweep
    linear_spline
    0,  // sweep the following shape from here ...
    1,  // _sd,  // ... up through here, then scale by 2*_sd
    4,  // the number of points making up the shape ...
    <pp1.x,pp1.z>,
    <pp2.x,pp2.z>
    <pp3.x,pp3.z>
    <pp1.x,pp1.z>
    no_shadow
    hollow 
       texture{
            pigment{color rgbt<0,0,1,.9>}
            // finish{ scDullMirror }
       }      
      /// interior_texture{
      ///      pigment{color rgbt<.71,.71,.71,1> }
      ///      finish{ Dull ambient .5 }
      /// }
       
// media on
       interior{                   
            media{ 
              method 3 
    			aa_threshold 
    			0.1 aa_level 2
    			samples 4 
    			intervals 1                            
    			emission rgb <0,0,1>
    			}
            }
   
   }            
// and media off */

/*       
   scale <1,2*_sd,1>
   translate <0, -_sd, 0>
    matrix <
     e1.x, e1.y, e1.z,
     e2.x, e2.y, e2.z,
     e3.x, e3.y, e3.z,
     dd.x, dd.y, dd.z
    >   
}
*/

