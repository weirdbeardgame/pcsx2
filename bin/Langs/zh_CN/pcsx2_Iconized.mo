Þ    G      T  a                9     Y   Õ  R   /  b     b   å  w   H  o   À  Í   0	     þ	  ~   
  Æ     `   á  Q   B       Ó   *  /   þ  M   .  ]   |  «   Ú  G     H   Î  d        |  b     Ï   q     A  5   É  6   ÿ  ÷   6     .  À   ³  Õ   t  ¡   J  r  ì  |   _  î   Ü    Ë  N  X  ÿ   §  Ü   §  ¤        )  á   Â  z  ¤   J   "  h   j"     Ó"  p   h#  G  Ù#     !%     ¥%     B&  Ù   Ê&     ¤'  í   6(  ¼   $)  Â   á)  ¡   ¤*     F+  *  á+  û   -  ß   .  \   è.  r   E/     ¸/  ï   C0  °   31  ©   ä1    2    4  f   .6  -   6  N   Ã6  _   7  \   r7  c   Ï7  h   38  s   8  ¦   9  ©   ·9     a:     ë:  9   ;  N   º;  n   	<     x<  /   =  C   F=  T   =  ²   ß=  C   >  a   Ö>  P   8?  z   ?  J   @  «   O@  p   û@  /   lA  /   A  ´   ÌA  e   B  ±   çB  Æ   C  y   `D  (  ÚD  k   F  È   oF  7  8G    pH     uI     J     4K     ÅK  ä   NL  q  3M  5   ¥N  \   ÛN  q   8O  W   ªO    P  |   Q     Q  q   7R  ³   ©R     ]S  Ò   ïS     ÂT     bU     ìU     tV  Ü   	W  é   æW  ×   ÐX  L   ¨Y  X   õY  _   NZ  ¸   ®Z     g[     ú[    |\                          	   :   A   ,         5      ;   )   0               1   @   6   4                    &      "   -      E       F   B   #          9   (           3   <   '   C       D   ?       %      !             2         8       =       /   .                              +       7                    G              >   
   $             *        'Ignore' to continue waiting for the thread to respond.
'Cancel' to attempt to cancel the thread.
'Terminate' to quit PCSX2 immediately.
 0 - Disables VU Cycle Stealing.  Most compatible setting! 1 - Default cyclerate. This closely matches the actual speed of a real PS2 EmotionEngine. 1 - Mild VU Cycle Stealing.  Lower compatibility, but some speedup for most games. 2 - Moderate VU Cycle Stealing.  Even lower compatibility, but significant speedups in some games. 2 - Reduces the EE's cyclerate by about 33%.  Mild speedup for most games with high compatibility. 3 - Maximum VU Cycle Stealing.  Usefulness is limited, as this will cause flickering visuals or slowdown in most games. 3 - Reduces the EE's cyclerate by about 50%.  Moderate speedup, but *will* cause stuttering audio on many FMVs. All plugins must have valid selections for %s to run.  If you are unable to make a valid selection due to missing plugins or an incomplete install of %s, then press Cancel to close the Configuration panel. Avoids memory card corruption by forcing games to re-index card contents after loading from savestates.  May not be compatible with all games (Guitar Hero). Check HDLoader compatibility lists for known games that have issues with this. (Often marked as needing 'mode 1' or 'slow DVD' Check this to force the mouse cursor invisible inside the GS window; useful if using the mouse as a primary control device for gaming.  By default the mouse auto-hides after 2 seconds of inactivity. Completely closes the often large and bulky GS window when pressing ESC or pausing the emulator. Enable this if you think MTGS thread sync is causing crashes or graphical errors. Enables automatic mode switch to fullscreen when starting or resuming emulation. You can still toggle fullscreen display at any time using alt-enter. Existing %s settings have been found in the configured settings folder.  Would you like to import these settings or overwrite them with %s default values?

(or press Cancel to select a different settings folder) Failed: Destination memory card '%s' is in use. Failed: Duplicate is only allowed to an empty PS2-Port or to the file system. Known to affect following games:
 * Bleach Blade Battler
 * Growlanser II and III
 * Wizardry Known to affect following games:
 * Digital Devil Saga (Fixes FMV and crashes)
 * SSX (Fixes bad graphics and crashes)
 * Resident Evil: Dead Aim (Causes garbled textures) Known to affect following games:
 * Mana Khemia 1 (Going "off campus")
 Known to affect following games:
 * Test Drive Unlimited
 * Transformers NTFS compression can be changed manually at any time by using file properties from Windows Explorer. NTFS compression is built-in, fast, and completely reliable; and typically compresses memory cards very well (this option is highly recommended). Note that when Framelimiting is disabled, Turbo and SlowMotion modes will not be available either. Note: Recompilers are not necessary for PCSX2 to run, however they typically improve emulation speed substantially. You may have to manually re-enable the recompilers listed above, if you resolve the errors. Notice: Due to PS2 hardware design, precise frame skipping is impossible. Enabling it will cause severe graphical errors in some games. Notice: Most games are fine with the default options. Notice: Most games are fine with the default options.  Out of Memory (sorta): The SuperVU recompiler was unable to reserve the specific memory ranges required, and will not be available for use.  This is not a critical error, since the sVU rec is obsolete, and you should use microVU instead anyway. :) PCSX2 is unable to allocate memory needed for the PS2 virtual machine. Close out some memory hogging background tasks and try again. PCSX2 requires a *legal* copy of the PS2 BIOS in order to run games.
You cannot use a copy obtained from a friend or the Internet.
You must dump the BIOS from your *own* Playstation 2 console. PCSX2 requires a PS2 BIOS in order to run.  For legal reasons, you *must* obtain a BIOS from an actual PS2 unit that you own (borrowing doesn't count).  Please consult the FAQs and Guides for further instructions. Playstation game discs are not supported by PCSX2.  If you want to emulate PSX games then you'll have to download a PSX-specific emulator, such as ePSXe or PCSX. Please ensure that these folders are created and that your user account is granted write permissions to them -- or re-run PCSX2 with elevated (administrator) rights, which should grant PCSX2 the ability to create the necessary folders itself.  If you do not have elevated rights on this computer, then you will need to switch to User Documents mode (click button below). Please select a valid BIOS.  If you are unable to make a valid selection then press Cancel to close the Configuration panel. Please select your preferred default location for PCSX2 user-level documents below (includes memory cards, screenshots, settings, and savestates).  These folder locations can be overridden at any time using the Plugin/BIOS Selector panel. Primarily targetting the EE idle loop at address 0x81FC0 in the kernel, this hack attempts to detect loops whose bodies are guaranteed to result in the same machine state for every iteration until a scheduled event triggers emulation of another unit.  After a single iteration of such loops, we advance to the time of the next event or the end of the processor's timeslice, whichever comes first. Removes any benchmark noise caused by the MTGS thread or GPU overhead.  This option is best used in conjunction with savestates: save a state at an ideal scene, enable this option, and re-load the savestate.

Warning: This option can be enabled on-the-fly but typically cannot be disabled on-the-fly (video will typically be garbage). Runs VU1 on its own thread (microVU1-only). Generally a speedup on CPUs with 3 or more cores. This is safe for most games, but a few games are incompatible and may hang. In the case of GS limited games, it may be a slowdown (especially on dual core CPUs). Setting higher values on this slider effectively reduces the clock speed of the EmotionEngine's R5900 core cpu, and typically brings big speedups to games that fail to utilize the full potential of the real PS2 hardware. Speedhacks usually improve emulation speed, but can cause glitches, broken audio, and false FPS readings.  When having emulation problems, disable this panel first. The PS2-slot %d has been automatically disabled.  You can correct the problem
and re-enable it at any time using Config:Memory cards from the main menu. The Presets apply speed hacks, some recompiler options and some game fixes known to boost speed.
Known important game fixes will be applied automatically.

--> Uncheck to modify settings manually (with current preset as base) The Presets apply speed hacks, some recompiler options and some game fixes known to boost speed.
Known important game fixes will be applied automatically.

Presets info:
1 -     The most accurate emulation but also the slowest.
3 --> Tries to balance speed with compatibility.
4 -     Some more aggressive hacks.
6 -     Too many hacks which will probably slow down most games.
 The specified path/directory does not exist.  Would you like to create it? The thread '%s' is not responding.  It could be deadlocked, or it might just be running *really* slowly. There is not enough virtual memory available, or necessary virtual memory mappings have already been reserved by other processes, services, or DLLs. This action will reset the existing PS2 virtual machine state; all current progress will be lost.  Are you sure? This command clears %s settings and allows you to re-run the First-Time Wizard.  You will need to manually restart %s after this operation.

WARNING!!  Click OK to delete *ALL* settings for %s and force-close the app, losing any current emulation progress.  Are you absolutely sure?

(note: settings for plugins are unaffected) This folder is where PCSX2 records savestates; which are recorded either by using menus/toolbars, or by pressing F1/F3 (save/load). This folder is where PCSX2 saves its logfiles and diagnostic dumps.  Most plugins will also adhere to this folder, however some older plugins may ignore it. This folder is where PCSX2 saves screenshots.  Actual screenshot image format and style may vary depending on the GS plugin being used. This hack works best for games that use the INTC Status register to wait for vsyncs, which includes primarily non-3D RPG titles. Games that do not use this method of vsync will see little or no speedup from this hack. This is the folder where PCSX2 saves your settings, including settings generated by most plugins (some older plugins may not respect this value). This recompiler was unable to reserve contiguous memory required for internal caches.  This error can be caused by low virtual memory resources, such as a small or disabled swapfile, or by another program that is hogging a lot of memory. This slider controls the amount of cycles the VU unit steals from the EmotionEngine.  Higher values increase the number of cycles stolen from the EE for each VU microprogram the game runs. This wizard will help guide you through configuring plugins, memory cards, and BIOS.  It is recommended if this is your first time installing %s that you view the readme and configuration guide. Updates Status Flags only on blocks which will read them, instead of all the time. This is safe most of the time, and Super VU does something similar by default. Vsync eliminates screen tearing but typically has a big performance hit. It usually only applies to fullscreen mode, and may not work with all GS plugins. Warning!  Changing plugins requires a complete shutdown and reset of the PS2 virtual machine. PCSX2 will attempt to save and restore the state, but if the newly selected plugins are incompatible the recovery may fail, and current progress will be lost.

Are you sure you want to apply settings now? Warning!  You are running PCSX2 with command line options that override your configured plugin and/or folder settings.  These command line options will not be reflected in the settings dialog, and will be disabled when you apply settings changes here. Warning!  You are running PCSX2 with command line options that override your configured settings.  These command line options will not be reflected in the Settings dialog, and will be disabled if you apply any changes here. Warning: Some of the configured PS2 recompilers failed to initialize and have been disabled: When checked this folder will automatically reflect the default associated with PCSX2's current usermode setting.  You are about to delete the formatted memory card '%s'. All data on this card will be lost!  Are you absolutely and quite positively sure? You can change the preferred default location for PCSX2 user-level documents here (includes memory cards, screenshots, settings, and savestates).  This option only affects Standard Paths which are set to use the installation default value. You may optionally specify a location for your PCSX2 settings here.  If the location contains existing PCSX2 settings, you will be given the option to import or overwrite them. Your system is too low on virtual resources for PCSX2 to run. This can be caused by having a small or disabled swapfile, or by other programs that are hogging resources. Zoom = 100: Fit the entire image to the window without any cropping.
Above/Below 100: Zoom In/Out
0: Automatic-Zoom-In untill the black-bars are gone (Aspect ratio is kept, some of the image goes out of screen).
  NOTE: Some games draw their own black-bars, which will not be removed with '0'.

Keyboard: CTRL + NUMPAD-PLUS: Zoom-In, CTRL + NUMPAD-MINUS: Zoom-Out, CTRL + NUMPAD-*: Toggle 100/0 Project-Id-Version: PCSX2 0.9.9
Report-Msgid-Bugs-To: https://github.com/PCSX2/pcsx2/issues
POT-Creation-Date: 2015-10-23 19:59+0200
PO-Revision-Date: 2014-12-13 23:54+0800
Last-Translator: Wei Mingzhi <whistler_wmz@users.sf.net>
Language-Team: 
Language: zh_CN
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
X-Poedit-KeywordsList: pxE;pxExpandMsg
X-Poedit-SourceCharset: utf-8
X-Poedit-Basepath: pcsx2\
X-Generator: Poedit 1.7.1
X-Poedit-SearchPath-0: pcsx2
X-Poedit-SearchPath-1: common
 "å¿½ç¥": ç»§ç»­ç­å¾è¿ç¨ååºã
"åæ¶": å°è¯åæ¶è¿ç¨ã
"ç»æ­¢": ç«å³éåº PCSX2ã
 0 - ç¦ç¨ VU å¨ææªç¨ãå¼å®¹æ§æé«! 1 - é»è®¤å¨æé¢çãå®å¨éç° PS2 å®æºææå¼æçå®ééåº¦ã 1 - è½»å¾® VU å¨ææªç¨ãå¼å®¹æ§è¾ä½ï¼ä½å¯¹å¤§å¤æ°æ¸¸ææä¸å®çæéææã 2 - ä¸­ç­ VU å¨ææªç¨ãå¼å®¹æ§æ´ä½ï¼ä½å¯¹ä¸äºæ¸¸ææè¾å¤§çæéææã 2 - å° EE å¨æé¢çåå°çº¦ 33%ãå¯¹å¤§å¤æ°æ¸¸ææè½»å¾®æéææï¼å¼å®¹æ§è¾é«ã 3 - æå¤§ç VU å¨ææªç¨ãå¯¹å¤§å¤æ°æ¸¸æå°é æå¾åéªçæéåº¦ææ¢ï¼ç¨éæéã 3 - å° EE å¨æé¢çåå°çº¦ 50%ãä¸­ç­æéææï¼ä½å°å¯¼è´å¾å¤ CG å¨ç»ä¸­çé³é¢åºç°é´æ­ã è¦è¿è¡ %sï¼æææä»¶å¿é¡»è®¾å®ä¸ºåæ³ãå¦æç±äºæä»¶ç¼ºå¤±æä¸å®æ´ç %s å®è£æ¨ä¸è½ååºåæ³éæ©ï¼è¯·åå»åæ¶å³é­éç½®é¢æ¿ã ä»¥å¼ºå¶æ¸¸æå¨è¯»åå³æ¶å­æ¡£åéæ°æ£ç´¢è®°å¿å¡åå®¹çæ¹å¼é¿åè®°å¿å¡æåãå¯è½ä¸ä¸æææ¸¸æå¼å®¹ (å¦ Guitar Hero ãåä»è±éã)ã è¯·åç HDLoader å¼å®¹æ§åè¡¨ä»¥è·åå¯ç¨æ­¤é¡¹ä¼åºç°é®é¢çæ¸¸æåè¡¨ã(éå¸¸æ è®°ä¸ºéè¦ 'mode 1' æ 'æ¢é DVD') éä¸­æ­¤é¡¹å¼ºå¶ GS çªå£ä¸­ä¸æ¾ç¤ºé¼ æ åæ ãå¨ä½¿ç¨é¼ æ æ§å¶æ¸¸ææ¶æ¯è¾æç¨ãé»è®¤ç¶æé¼ æ å¨ 2 ç§ä¸æ´»å¨åéèã å¨æ ESC ææèµ·æ¨¡æå¨æ¶å½»åºå³é­ GS çªå£ã å¦æ¨è®¤ä¸º MTGS çº¿ç¨åæ­¥å¯¼è´å´©æºæå¾åéè¯¯ï¼è¯·å¯ç¨æ­¤é¡¹ã å¯å¨ææ¢å¤æ¨¡ææ¶èªå¨åæ¢è³å¨å±ãæ¨å¯ä»¥ä½¿ç¨ Alt+Enter éæ¶åæ¢å¨å±æçªå£æ¨¡å¼ã éç½®çæä»¶å¤¹ä¸­å·²æ %s è®¾ç½®ãæ¨æ³å¯¼å¥è¿äºè®¾ç½®è¿æ¯ç¨ %s é»è®¤è®¾ç½®è¦çå®ä»¬?

(æåå»åæ¶éæ©ä¸ä¸ªä¸åçè®¾ç½®æä»¶å¤¹) å¤±è´¥: ç®æ è®°å¿å¡ '%s' æ­£å¨è¢«ä½¿ç¨ã å¤±è´¥: åªåè®¸å¤å¶å°ä¸ä¸ªç©ºç PS2 ç«¯å£ææä»¶ç³»ç»ã å·²ç¥å¯¹ä»¥ä¸æ¸¸æææ:
 * æ­»ç¥ååæå£«
 * æ¢¦å¹»éªå£« 2 å 3
 * å·«æ¯ å·²ç¥å½±åä»¥ä¸æ¸¸æ:
 * æ°ç æ¶é­ä¼ è¯´ (ä¿®æ­£ CG åå´©æºé®é¢)
 * æéæ»éª (ä¿®æ­£å¾åéè¯¯åå´©æºé®é¢)
 * çåå±æº: æ­»äº¡ç®æ  (å¯¼è´çº¹çæ··ä¹±) å·²ç¥å½±åä»¥ä¸æ¸¸æ:
 * Mana Khemia 1 (å­¦æ ¡çç¼éæ¯å£«)
 å·²ç¥å½±åä»¥ä¸æ¸¸æ:
 * Test Drive Unlimited (æ éè¯é©¾ 2)
 * Transformers (åå½¢éå) NTFS åç¼©å¯ä»¥éæ¶ä½¿ç¨ Windows èµæºç®¡çå¨ä¸­çæä»¶å±æ§æ´æ¹ã NTFS åç¼©æ¯åç½®ãé«æãå¯é çï¼éå¸¸å¯¹äºè®°å¿å¡æä»¶åç¼©æ¯éå¸¸é« (å¼ºçå»ºè®®ä½¿ç¨æ­¤éé¡¹)ã æ³¨æ: å¦éå¸§è¢«ç¦ç¨ï¼å¿«éæ¨¡å¼åæ¢å¨ä½æ¨¡å¼å°ä¸å¯ç¨ã æ³¨æ: éç¼è¯å¨å¯¹ PCSX2 éå¿éï¼ä½æ¯å®ä»¬éå¸¸å¯å¤§å¤§æåæ¨¡æéåº¦ãå¦éè¯¯å·²è§£å³ï¼æ¨å¯è½è¦æå¨éæ°å¯ç¨ä»¥ä¸ååºçéç¼è¯å¨ã æ³¨æ: ç±äº PS2 ç¡¬ä»¶è®¾è®¡ï¼ä¸å¯è½åç¡®è·³å¸§ãå¯ç¨æ­¤éé¡¹å¯è½å¨æ¸¸æä¸­å¯¼è´å¾åéè¯¯ã æ³¨: å¤§å¤æ°æ¸¸æä½¿ç¨é»è®¤éé¡¹å³å¯ã æ³¨: å¤§å¤æ°æ¸¸æä½¿ç¨é»è®¤éé¡¹å³å¯ã åå­æº¢åº: SuperVU éç¼è¯å¨æ æ³ä¿çæéçæå®åå­èå´ï¼ä¸å°ä¸å¯ç¨ãè¿ä¸æ¯ä¸ä¸ªä¸¥ééè¯¯ï¼sVU éç¼è¯å¨å·²è¿æ¶ï¼æ¨åºè¯¥ä½¿ç¨ microVUã:) PCSX2 æ æ³åé PS2 èææºæéåå­ãè¯·å³é­ä¸äºå ç¨åå­çåå°ä»»å¡åéè¯ã PCSX2 éè¦ä¸ä¸ªåæ³ç PS2 BIOS å¯æ¬æ¥è¿è¡æ¸¸æãä½¿ç¨éæ³å¤å¶æä¸è½½çå¯æ¬ä¸ºä¾µæè¡ä¸ºãæ¨å¿é¡»ä»æ¨èªå·±ç Playstation 2 å®æºä¸­åå¾ BIOSã PCSX2 éè¦ä¸ä¸ª PS2 BIOS æå¯ä»¥è¿è¡ãç±äºæ³å¾é®é¢ï¼æ¨å¿é¡»ä»ä¸å°å±äºæ¨ç PS2 å®æºä¸­åå¾ä¸ä¸ª BIOS æä»¶ãè¯·åèå¸¸è§é®é¢åæç¨ä»¥è·åè¿ä¸æ­¥çè¯´æã PCSX2 ä¸æ¯æ Playstation 1 æ¸¸æãå¦ææ¨æ³æ¨¡æ PS1 æ¸¸æè¯·ä¸è½½ä¸ä¸ª PS1 æ¨¡æå¨ï¼å¦ ePSXe æ PCSXã è¯·ç¡®ä¿è¿äºæä»¶å¤¹å·²è¢«å»ºç«ä¸æ¨çç¨æ·è´¦æ·å¯¹å®ä»¬æåå¥æé -- æä½¿ç¨ç®¡çåæééæ°è¿è¡ PCSX2 (å¯ä»¥ä½¿ PCSX2 è½å¤èªå¨å»ºç«å¿è¦çæä»¶å¤¹)ãå¦ææ¨æ²¡ææ­¤è®¡ç®æºçç®¡çåæéï¼æ¨éè¦åæ¢è³ç¨æ·æä»¶æ¨¡å¼ (åå»ä¸é¢çæé®)ã è¯·éæ©ä¸ä¸ªåæ³ç BIOSãå¦ææ¨ä¸è½ä½åºåæ³çéæ©è¯·åå»åæ¶æ¥å³é­éç½®é¢æ¿ã è¯·å¨ä¸é¢éæ©æ¨åå¥½ç PCSX2 ç¨æ·ææ¡£é»è®¤ä½ç½® (åæ¬è®°å¿å¡ãæªå¾ãè®¾ç½®éé¡¹åå³æ¶å­æ¡£)ãè¿äºæä»¶å¤¹ä½ç½®å¯ä»¥éæ¶å¨ "æä»¶å BIOS éæ©" é¢æ¿ä¸­æ´æ¹ã ä¸»è¦éå¯¹ä½äºåæ ¸å°å 0x81FC0 ç EE ç©ºé²å¾ªç¯ï¼æ­¤ Hack è¯å¾æ£æµå¾ªç¯ä½å¨ä¸ä¸ªå¦å¤çæ¨¡æååè®¡åçäºä»¶å¤çè¿ç¨ä¹åä¸ä¿è¯äº§çç¸åç»æçå¾ªç¯ãå¨ä¸æ¬¡å¾ªç¯ä½æ§è¡ä¹åï¼å°ä¸ä¸äºä»¶çæ¶é´æå¤çå¨çæ¶é´çç»ææ¶é´ (å­°æ©) ååºæ´æ°ã ç¦ç¨å¨é¨ç± MTGS çº¿ç¨æ GPU å¼éå¯¼è´çæµè¯ä¿¡æ¯ãæ­¤éé¡¹å¯ä¸å³æ¶å­æ¡£éåä½¿ç¨: å¨çæ³çåºæ¯ä¸­å­æ¡£ï¼å¯ç¨æ­¤éé¡¹ï¼è¯»æ¡£ã

è­¦å: æ­¤éé¡¹å¯ä»¥å³æ¶å¯ç¨ä½éå¸¸ä¸è½å³æ¶å³é­ (éå¸¸ä¼å¯¼è´å¾åæå)ã å¨åç¬ççº¿ç¨æ¯è¿è¡ VU1 (ä»é microVU1)ãéå¸¸å¨ä¸æ ¸ä»¥ä¸ CPU ä¸­ææéææãæ­¤éé¡¹å¯¹å¤§å¤æ°æ¸¸ææ¯å®å¨çï¼ä½ä¸é¨åæ¸¸æå¯è½ä¸å¼å®¹æå¯¼è´æ²¡æååºãå¯¹äºåéäº GS çæ¸¸æï¼å¯è½ä¼é ææ§è½ä¸é (ç¹å«æ¯å¨åæ ¸ CPU ä¸)ã æé«æ­¤æ°å¼å¯åå°ææå¼æç R5900 CPU çæ¶ééåº¦ï¼éå¸¸å¯¹äºæªå®å¨ä½¿ç¨ PS2 å®æºç¡¬ä»¶å¨é¨æ½è½çæ¸¸ææè¾å¤§æéææã éåº¦ Hack éå¸¸å¯ä»¥æåæ¨¡æéåº¦ï¼ä½ä¹å¯è½å¯¼è´éè¯¯ãå£°é³é®é¢æèå¸§ãå¦æ¨¡ææé®é¢è¯·åå°è¯ç¦ç¨æ­¤é¢æ¿ã %d ææ§½ä¸çè®°å¿å¡å·²èªå¨è¢«ç¦ç¨ãæ¨å¯ä»¥éæ¶å¨ä¸»èåä¸çéç½®:è®°å¿å¡ä¸­æ¹æ­£é®é¢å¹¶éæ°å¯ç¨è®°å¿å¡ã é¢ç½®å°å½±åéåº¦ Hackãä¸äºéç¼è¯å¨éé¡¹åä¸äºå·²ç»å¯æåéåº¦çæ¸¸æç¹æ®ä¿®æ­£ã
å·²ç¥çæ¸¸æç¹æ®ä¿®æ­£ ("è¡¥ä¸") å°èªå¨è¢«åºç¨ã

--> åæ¶æ­¤é¡¹å¯æå¨ä¿®æ¹è®¾ç½® (åºäºå½åé¢ç½®) é¢ç½®å°å½±åéåº¦ Hackãä¸äºéç¼è¯å¨éé¡¹åä¸äºå·²ç»å¯æåéåº¦çæ¸¸æç¹æ®ä¿®æ­£ã
å·²ç¥çæ¸¸æç¹æ®ä¿®æ­£ ("è¡¥ä¸") å°èªå¨è¢«åºç¨ã

é¢ç½®ä¿¡æ¯:
1 -    æ¨¡æç²¾ç¡®åº¦æé«ï¼ä½éåº¦æä½ã
3 --> è¯å¾å¹³è¡¡éåº¦åå¼å®¹æ§ã
4 -    ä¸äºæ´å¤ç Hackã
6 -    è¿å¤ Hackï¼æå¯è½ææ¢å¤§å¤æ°æ¸¸æçéåº¦ã
 æå®çè·¯å¾/ç®å½ä¸å­å¨ãæ¯å¦éè¦åå»º? çº¿ç¨ '%s' æ²¡æååºãå®å¯è½åºç°æ­»éï¼æå¯è½ä»ä»æ¯è¿è¡å¾*éå¸¸*æ¢ã æ²¡æè¶³å¤çèæåå­å¯ç¨ï¼ææéçèæåå­æ å°å·²ç»è¢«å¶å®è¿ç¨ãæå¡æ DLL ä¿çã æ­¤å¨ä½å°å¤ä½å½åç PS2 èææºç¶æï¼å½åè¿åº¦å°ä¸¢å¤±ãæ¯å¦ç¡®è®¤? æ­¤å½ä»¤å°æ¸é¤ %s çè®¾ç½®ä¸åè®¸æ¨éæ°è¿è¡é¦æ¬¡è¿è¡åå¯¼ãæ¨éè¦å¨æ­¤æä½å®æåéæ°å¯å¨ %sã

è­¦å!! åå»ç¡®å®å°å é¤å¨é¨ %s çè®¾ç½®ä¸å¼ºå¶å³é­åºç¨ç¨åºï¼å½åæ¨¡æè¿åº¦å°ä¸¢å¤±ãæ¯å¦ç¡®å®?

(æ³¨: æä»¶è®¾ç½®å°ä¸åå½±å) æ­¤æä»¶å¤¹æ¯ PCSX2 ä¿å­å³æ¶å­æ¡£çä½ç½®ï¼å³æ¶å­æ¡£å¯ä½¿ç¨èå/å·¥å·æ æ F1/F3 (ä¿å­/è¯»å) ä½¿ç¨ã æ­¤æä»¶å¤¹æ¯ PCSX2 ä¿å­æ¥å¿è®°å½åè¯æ­è½¬å¨çä½ç½®ãå¤§å¤æ°æä»¶ä¹å°ä½¿ç¨æ­¤æä»¶å¤¹ï¼ä½æ¯ä¸äºæ§çæä»¶å¯è½ä¼å¿½ç¥å®ã æ­¤æä»¶å¤¹æ¯ PCSX2 ä¿å­æªå¾çä½ç½®ãå®éæªå¾æ ¼å¼åé£æ ¼å¯¹äºä¸åç GS æä»¶å¯è½ä¸åã æ­¤éé¡¹å¯¹äºä½¿ç¨ INTC ç¶æå¯å­å¨æ¥ç­å¾åç´åæ­¥çæ¸¸æææè¾å¥½ï¼åæ¬ä¸äºä¸»è¦ç 3D RPG æ¸¸æãå¯¹äºä¸ä½¿ç¨æ­¤æ¹æ³çæ¸¸ææ²¡ææéææã è¿æ¯ PCSX2 ä¿å­æ¨çè®¾ç½®éé¡¹çæä»¶å¤¹ï¼åæ¬å¤§å¤æ°æä»¶çæçè®¾ç½®éé¡¹ (æ­¤éé¡¹å¯¹äºä¸äºæ§çæä»¶å¯è½æ æ)ã éç¼è¯å¨æ æ³ä¿çåé¨ç¼å­æéçè¿ç»­åå­ç©ºé´ãæ­¤éè¯¯å¯è½æ¯ç±èæåå­èµæºä¸è¶³å¼èµ·ï¼å¦äº¤æ¢æä»¶è¿å°ææªä½¿ç¨äº¤æ¢æä»¶ãæä¸ªå¶å®ç¨åºæ­£å ç¨è¿å¤§åå­ã æ­¤éé¡¹æ§å¶ VU ååä»ææå¼ææªç¨çæ¶éå¨ææ°ç®ãè¾é«æ°å¼å°å¢å åä¸ªè¢«æ¸¸ææ§è¡ç VU å¾®ç¨åºä» EE æªç¨çå¨ææ°ç®ã æ­¤åå¯¼å°å¼å¯¼æ¨éç½®æä»¶ãè®°å¿å¡å BIOSãå¦ææ¨æ¯ç¬¬ä¸æ¬¡è¿è¡ %sï¼å»ºè®®æ¨åæ¥çèªè¿°æä»¶åéç½®è¯´æã ä»å¨æ å¿ä½è¢«è¯»åæ¶æ´æ°ï¼èä¸æ¯æ»æ¯æ´æ°ãæ­¤éé¡¹éå¸¸æ¯å®å¨çï¼Super VU é»è®¤ä¼ä»¥ç¸ä¼¼çæ¹å¼å¤çã åç´åæ­¥å¯ä»¥æ¶é¤è±å±ä½éå¸¸å¯¹æ§è½æè¾å¤§å½±åãéå¸¸ä»åºç¨äºå¨å±å¹æ¨¡å¼ï¼ä¸ä¸ä¸å®å¯¹ææç GS æä»¶é½ææã è­¦å! æ´æ¢æä»¶éè¦å½»åºå³é­å¹¶éæ°å¯å¨ PS2 èææºãPCSX2 å°å°è¯ä¿å­å³æ¶å­æ¡£å¹¶è¯»åï¼ä½å¦ææ°éæ©çæä»¶ä¸å¼å®¹å°å¤±è´¥ï¼å½åè¿åº¦å°ä¸¢å¤±ã

æ¯å¦ç¡®è®¤åºç¨è¿äºè®¾ç½®? è­¦å! æ¨æ­£å¨ä½¿ç¨å½ä»¤è¡éé¡¹è¿è¡ PCSX2ï¼è¿å°è¦çæ¨å·²éç½®çæä»¶ææä»¶å¤¹è®¾å®ãè¿äºå½ä»¤è¡éé¡¹å°ä¸ä¼å¨è®¾ç½®å¯¹è¯æ¡ä¸­åæ ï¼ä¸å¦ææ¨æ´æ¹äºä»»ä½éé¡¹çè¯å½ä»¤è¡éé¡¹å°å¤±æã è­¦å! æ¨æ­£å¨ä½¿ç¨å½ä»¤è¡éé¡¹è¿è¡ PCSX2ï¼è¿å°è¦çæ¨å·²éç½®çè®¾å®ãè¿äºå½ä»¤è¡éé¡¹å°ä¸ä¼å¨è®¾ç½®å¯¹è¯æ¡ä¸­åæ ï¼ä¸å¦ææ¨æ´æ¹äºä»»ä½éé¡¹çè¯å½ä»¤è¡éé¡¹å°å¤±æã è­¦å: é¨åå·²éç½®ç PS2 éç¼è¯å¨åå§åå¤±è´¥ä¸å·²è¢«ç¦ç¨ã éä¸­æ¶æ­¤æä»¶å¤¹å°èªå¨åæ å½å PCSX2 ç¨æ·è®¾ç½®éé¡¹ç¸å³çé»è®¤å¼ã å³å°å é¤å·²æ ¼å¼åçè®°å¿å¡ '%s'ãæ­¤è®°å¿å¡ä¸­æææ°æ®å°ä¸¢å¤±! æ¯å¦ç¡®å®? æ¨å¯ä»¥å¨æ­¤æ´æ¹ PCSX2 ç¨æ·ææ¡£çé»è®¤ä½ç½® (åæ¬è®°å¿å¡ãæªå¾ãè®¾ç½®éé¡¹åå³æ¶å­æ¡£)ãæ­¤éé¡¹ä»å¯¹ç±å®è£æ¶çé»è®¤å¼è®¾å®çæ åè·¯å¾ææã æ¨å¯ä»¥æå®ä¸ä¸ªæ¨ç PCSX2 è®¾ç½®éé¡¹æå¨ä½ç½®ãå¦ææ­¤ä½ç½®åå«å·²æç PCSX2 è®¾ç½®ï¼æ¨å¯ä»¥éæ©å¯¼å¥æè¦çå®ä»¬ã æ¨çç³»ç»æ²¡æè¶³å¤çèµæºè¿è¡ PCSX2ãå¯è½æ¯ç±äºäº¤æ¢æä»¶è¿å°ææªä½¿ç¨ï¼æå¶å®å ç¨èµæºçç¨åºã ç¼©æ¾ = 100: å¾åéåçªå£å¤§å°ï¼æ ä»»ä½è£åªã
å¤§äºæå°äº 100: æ¾å¤§/ç¼©å°ã
0: èªå¨æ¾å¤§ç´å°é»æ¡æ¶å¤± (çºµæ¨ªæ¯å°è¢«ä¿æï¼é¨åå¾åå°ä½äºå±å¹å¤é¢)ã
 æ³¨: ä¸äºæ¸¸ææå¨ç»å¶é»æ¡ï¼è¿ç§æåµé»æ¡å°ä¸ä¼è¢«ç§»é¤ã

é®ç: Ctrl+å°é®çå å·: æ¾å¤§ï¼Ctrl+å°é®çåå·: ç¼©å°ï¼Ctrl+å°é®çæå·: å¨ 100 å 0 ä¹é´åæ¢ 