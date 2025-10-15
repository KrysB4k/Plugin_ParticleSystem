# Particle System Plugin

## General Links
- [Online Plugin Manual](https://krysb4k.github.io/ParticleManual/index.html)
- [Particle-Module-Templates](https://github.com/KrysB4k/Particle-Module-Templates) (repository of example/demo module scripts)

## 1. What is Plugin_ParticleSystem?
It is a plugin for the **TRNG + FLEP** engine enhancements of **Tomb Raider Level Editor** (TRLE). It adds a dynamic particle engine that is under full user scripting control. It allows TRLE builders to create custom particle effects, going far beyond the capabilities of FLEP or TRNG.

The particle engine is augumented with a responsive and extensive **Scripting API** (Application Programming Interface), which allows direct communication with the plugin, through Lua. Users can write custom **Lua scripts** that implement particle effects in the form of **modules**, which can then be used in TRLE projects or shared among other members of the community.


## Feature Showcase - example particle effects achievable with the plugin

### Animated particle effects
<a href="http://www.youtube.com/watch?feature=player_embedded&v=-707H-99Pw4" target="_blank">
	<img src="https://img.youtube.com/vi/-707H-99Pw4/0.jpg"alt="Video 1" width="240" height="180" border="10" />
</a>

### Mesh particles - use 3D meshes for particles
<a href="http://www.youtube.com/watch?feature=player_embedded&v=yqc19lnccN4" target="_blank">
	<img src="https://img.youtube.com/vi/yqc19lnccN4/0.jpg"alt="Video 2" width="240" height="180" border="10" />
</a>

### Autumn leaves
<a href="http://www.youtube.com/watch?feature=player_embedded&v=yMgZJK-JQsk" target="_blank">
	<img src="https://img.youtube.com/vi/yMgZJK-JQsk/0.jpg"alt="Video 5" width="240" height="180" border="10" />
</a>

### Better AddEffects - attaching custom particles to meshes of moveables
<a href="http://www.youtube.com/watch?feature=player_embedded&v=lXhmGMtjqaU" target="_blank">
	<img src="https://img.youtube.com/vi/lXhmGMtjqaU/0.jpg"alt="Video 6" width="240" height="180" border="10" />
</a>
<a href="http://www.youtube.com/watch?feature=player_embedded&v=H9pxqTsz1T4" target="_blank">
	<img src="https://img.youtube.com/vi/H9pxqTsz1T4/0.jpg"alt="Video 6" width="240" height="180" border="10" />
</a>

### Particles with complex custom behavior - collisions with room geometry, spawning shockwaves, emitting lights etc.
<a href="http://www.youtube.com/watch?feature=player_embedded&v=qSH70ALZ8L4" target="_blank">
	<img src="https://img.youtube.com/vi/qSH70ALZ8L4/0.jpg"alt="Video 7" width="240" height="180" border="10" />
</a>
<a href="http://www.youtube.com/watch?feature=player_embedded&v=Px5klrf-O00" target="_blank">
	<img src="https://img.youtube.com/vi/Px5klrf-O00/0.jpg"alt="Video 7" width="240" height="180" border="10" />
</a>

### ScreenSpace particles - particles which exist in the game screen foreground
<a href="http://www.youtube.com/watch?feature=player_embedded&v=PbC29phy0ks" target="_blank">
	<img src="https://img.youtube.com/vi/PbC29phy0ks/0.jpg"alt="Video 8" width="240" height="180" border="10" />
</a>
<a href="http://www.youtube.com/watch?feature=player_embedded&v=qSMAS30MvZM" target="_blank">
	<img src="https://img.youtube.com/vi/qSMAS30MvZM/0.jpg"alt="Video 8" width="240" height="180" border="10" />
</a>

### Scripting custom weapon types or traps:
<a href="http://www.youtube.com/watch?feature=player_embedded&v=jMAYHiv4PkQ" target="_blank">
	<img src="https://img.youtube.com/vi/jMAYHiv4PkQ/0.jpg"alt="Video 8" width="240" height="180" border="10" />
</a>



## 2. What engine version is this plugin for?
The targeted engine version is **TRNG 1.3.0.7**, combined with the FLEP patching system for best results.


## 3. How do you install this plugin?
The plugin's binary builds are bundled as [**releases**](https://github.com/KrysB4k/Plugin_ParticleSystem/releases) on this repository. Always download the plugin ZIP package from the latest available release for bugfixes and improvements!

The exact steps of plugin installation depend on whether you are managing your TRLE project with TombIDE from the [Tomb Editor suite](https://github.com/TombEngine/TombEditorReleases/releases) or with old NG Center.

### 3.1 When using TombIDE
Download the **Plugin_ParticleSystem** ZIP archive from the latest release. Open your TRLE project in TombIDE, go to the **Plugin Manager** tab and click **Install plugin from .zip archive**. In the file explorer, find the plugin ZIP file and select it. The plugin should now be installed and ready to use.

### 3.2 When using NG Center
Download the **Plugin_ParticleSystem** ZIP archive from the latest release. Open NG Center and switch to your current TRLE project, if necessary. Go to the **Plugins** tab, then click **Install new Plugin**. Select **Install from ZIP**, locate the plugin ZIP in the file explorer and select it. The plugin should now be installed and ready to use.


## 4. How to use this plugin?
This plugin is very robust and its usage cannot be explained in a single paragraph of text. A dedicated series of manuals explaining how to use it was created. These manuals are being written an will be gradually updated with more contents in future releases. Each plugin release comes bundled with a copy of the manual available at the time of the release (in the **Manual** folder). However, the most up-to-date version will be hosted online as a GitHub Pages website, under this link: [Online Plugin Manual](https://krysb4k.github.io/ParticleManual/index.html)

How you should use this plugin depends on whether you intend to script your own effects in Lua, or use effects shared by other users.

### 4.1 Using effects made by others
To learn how to import existing **effect modules** into your TRLE project, follow the [**Builder Path**](https://krysb4k.github.io/ParticleManual/builder/BuilderIntro.html) in the manual. It describes the basic usage of the plugin for builder who will not code custom effects. Because this is a rather simple process, it is a short read, consisting of 4 pages in total. Once you finish the **Builder Path** and you want to try your strengths in coding custom effects, you can check out the **Coder Path**.

### 4.2 Scripting your own effects
To learn how to script your own effects, follow the [**Coder Path**](https://krysb4k.github.io/ParticleManual/coder/CoderIntro.html) in the manual. It describes everything about the scripting process in great detail, including a basic rundown of the Lua scripting language and how to script your first **effect modules** and use them in your levels. Although prior experience with Lua or programming in general can help, it is not required. The **Coder Path** is tailored to any skill level, including complete beginners. That being said, a solid grasp of more advanced TRNG scripting (Global Triggers, Trigger Groups, Organizers, Conditions) is certainly recommended for this path.

The **Coder Path** is still in development and new chapters will be added gradually, when ready, so be sure to check out the online manual version ever so often!

Coming soon, an **API Reference** will also be hosted and updated with new functionalities and features in future plugin releases.


## 5. I found a bug in the plugin / I would like to request some feature. Where can I report it?
We mainly accept bug reports on the [Issues page](https://github.com/KrysB4k/Plugin_ParticleSystem/issues) of this repository. Reporting an issue requires registering a (free) GitHub account. After that, you simply click the "New Issue" button, following the examples for a [bug report](https://github.com/KrysB4k/Plugin_ParticleSystem/issues/11) or [feature request](https://github.com/KrysB4k/Plugin_ParticleSystem/issues/12) as guidelines.

Additionally, we will accept bug reports in the Tomb Raider Forums thread pertaining to this plugin:

Reports through other communication routes (e.g. personal messages, Discord chats) can be given, but we still strongly recommend creating an Issue entry in this repository, or making a post in the TRF thread, to keep everything tidy and organized.


## 6. Credits
This plugin was co-developed by:
- Krys [@KrysB4k](https://github.com/KrysB4k)
- ChocolateFan [@asasas95000](https://github.com/asasas9500)

On top of that, a big thank you goes out to [Martini](https://www.youtube.com/@SpikeyTombRaider) who did very extensive testing of this plugin and helped find many bugs in the process. This plugin would not have been possible without your input!

I would also like to thank several people in the TRLE community, who have supported me in the process of working on this plugin:
- MarlenaCrystal
- Dermahn [@Dermahn](https://github.com/Dermahn)
- Raider99
- Klona
- Dinne
- JoeyQuint [@Joey79100](https://github.com/Joey79100)
- BigBonesParker
