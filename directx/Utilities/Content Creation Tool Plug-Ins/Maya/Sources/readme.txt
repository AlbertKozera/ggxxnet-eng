Installation Instructions
=========================

In this release, it is automatically installed by 'setup.' [See manual install for other options] Though the user must still activate the plugin in Maya


1. Launch Maya
2. From the main menu select "Window->Settings/Preferences->Plugin Manager..."
3. Locate "D3DMaya#ExportX.mll" in a seperate subsection at the bottom of the list 
	check the boxes next to "loaded" and "auto load" and click the Close button
4. You should now see a "DirectX" main menu item indicating the plug-in was successfully installed

Manual Installation Instructions
================================
If you are recompiling the code or are having trouble with the automatic install the following are instructions

****For Maya5****

Delete List (make sure old installs are gone)

D3DMaya5ExportX.mll 		from	$(MAYA5_DIR)Bin
D3DMaya5ExportX.mll 		from	$(MAYA5_DIR)Bin\Plugins
DXCC.dll 			from	$(MAYA5_DIR)Bin
DXCC.dll 			from	$(MAYA5_DIR)Bin\Plugins
DirectX.mel 			from	$(MAYA5_DIR)Scripts\startup
AEDirectXShaderTemplate.mel 	from	$(MAYA5_DIR)Scripts\AETemplates
DirectX.xpm 			from	$(MAYA5_DIR)Icons
DirectX.bmp 			from	$(MAYA5_DIR)Icons
render_DirectXShader.xpm 	from	$(MAYA5_DIR)Icons
render_DirectXShader.bmp 	from	$(MAYA5_DIR)Icons

Copy List

D3DMaya5ExportX.mll		to	$(DXSDK_DIR)Utilities\Content Creation Tool Plug-Ins\Maya\Binary5\Plug-Ins
DXCC.dll			to	$(DXSDK_DIR)Utilities\Content Creation Tool Plug-Ins\Maya\Binary5\Plug-Ins
DirectX.mel			to	$(DXSDK_DIR)Utilities\Content Creation Tool Plug-Ins\Maya\Binary5\Scripts
AEDirectXShaderTemplate.mel	to	$(DXSDK_DIR)Utilities\Content Creation Tool Plug-Ins\Maya\Binary5\Scripts
render_DirectXShader.xpm	to	$(DXSDK_DIR)Utilities\Content Creation Tool Plug-Ins\Maya\Binary5\Icons
DirectX.xpm			to	$(DXSDK_DIR)Utilities\Content Creation Tool Plug-Ins\Maya\Binary5\Icons

then 

Place into the directory
$(ProgramFiles)\Common Files\AliasWavefront Shared\Modules\maya\5.0\
a file with an arbitrary name and the following line as content
+ D3DMaya5ExportX 5.0 C:\Program Files\Microsoft DirectX 9.0 SDK (October 2004)\Utilities\Content Creation Tool Plug-Ins\Maya\Binary5

****For Maya6****

Delete List (make sure old installs are gone)

D3DMaya6ExportX.mll 		from	$(MAYA6_DIR)Bin
D3DMaya6ExportX.mll 		from	$(MAYA6_DIR)Bin\Plugins
DXCC.dll 			from	$(MAYA6_DIR)Bin
DXCC.dll 			from	$(MAYA6_DIR)Bin\Plugins
DirectX.mel 			from	$(MAYA6_DIR)Scripts\startup
AEDirectXShaderTemplate.mel 	from	$(MAYA6_DIR)Scripts\AETemplates
DirectX.xpm 			from	$(MAYA6_DIR)Icons
DirectX.bmp 			from	$(MAYA6_DIR)Icons
render_DirectXShader.xpm 	from	$(MAYA6_DIR)Icons
render_DirectXShader.bmp 	from	$(MAYA6_DIR)Icons

Copy List

D3DMaya6ExportX.mll		to	$(DXSDK_DIR)Utilities\Content Creation Tool Plug-Ins\Maya\Binary6\Plug-Ins
DXCC.dll			to	$(DXSDK_DIR)Utilities\Content Creation Tool Plug-Ins\Maya\Binary6\Plug-Ins
DirectX.mel			to	$(DXSDK_DIR)Utilities\Content Creation Tool Plug-Ins\Maya\Binary6\Scripts
AEDirectXShaderTemplate.mel	to	$(DXSDK_DIR)Utilities\Content Creation Tool Plug-Ins\Maya\Binary6\Scripts
render_DirectXShader.xpm	to	$(DXSDK_DIR)Utilities\Content Creation Tool Plug-Ins\Maya\Binary6\Icons
DirectX.xpm			to	$(DXSDK_DIR)Utilities\Content Creation Tool Plug-Ins\Maya\Binary6\Icons

then

Place into the directory
$(ProgramFiles)\Common Files\AliasWavefront Shared\Modules\maya\6.0 
a file with an arbitrary name and the following line as content
+ D3DMaya6ExportX 5.0 C:\Program Files\Microsoft DirectX 9.0 SDK (October 2004)\Utilities\Content Creation Tool Plug-Ins\Maya\Binary6

User Guide
==========

The Preview Pipeline defines a new material type which encapsulates a D3DX Effect file (*.fx). 
Maya objects which are shaded with this "DirectX Shader" material will be rendered within the
Preview Pipeline viewer window using the associated Effect.

To create a "DirectX Shader" material:

1. From the main menu select "Window->Rendering Editors->Hypershade..." to open the Hypershade
   window.
2. Verify that "Create Materials" is selected within the creation dropdown menu
3. Click "DirectX Shader" to create an empty material. At this point no .fx file is associated
   with the material.
4. Open the Attribute Editor and select the tab for the new material (the default name is
   "DirectXShader1"). 
5. Click the "Select and Customize FX-File" button to open the FX Editor dialog. Using this dialog
   you can select a .fx file for the material and adjust Effect parameters. Click "Open" to
   select a .fx file and notice after a file is selected that the Effect's editable parameters 
   appear in the listbox. Click "Done" when finished.
6. To apply the material Middle-mouse drag the material from the Hypershade window onto an object.
   
The Preview Pipeline viewer window should now be rendering the object using the associated effect.
The viewer window can either be a standard floating window, in which you can move and adjust the 
window as normal; or can be bound to a Maya panel. To adjust the viewer window mode select an option 
from the "DirectX->Viewers..." main menu item. Note that the camera for the viewer window is 
automatically tied to Maya's perspective view.


Notes
======

Capturing Skinning
skinning is not on by default.  Turn it on though the menu "DirectX->Capture Skinning".
Then refresh the current scene or the particular model with "DirectX->Rebuild*"
Note though that the skinned models will appear in their default pose since the viewer does not support skinning in this release.
The Preview Pipeline only supports smooth skinning.  Results are not garanteed for other methods.

Capturing Animation
Animation is gathered whenever the "DirectX->Gather Animation" menu is selected.  
Note that the viewer does not support animation.
Mview now works with exported files.  The exporter was changed to reflect the MView standard.

Inverted Geometry
The Preview Pipeline captures geometry as is.  It does correct for winding order when the mesh's world matrix determinant is negative.
But it does not correct the geometry based on render flags.  The following "render stat" flags may appear as inverted geometry "Double Sided" and "Opposite."  The artist should have these setting off and instead use the "Edit Polygon->Normals->Reverse" command instead to correctly alter the geometry.