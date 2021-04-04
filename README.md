# esp32-table
Arduino project to drive a infinity-mirror led table with ~600 leds. <br> 
<br>
![Led Table](https://i.imgur.com/47Dju2b.png)

With the [accompanying app](https://github.com/GoodGuyMe/android-table) or through API calls it's possible to change the lighting.
Use the folling GET Requests to change the settings on the table:
* /mode?m=...
  * rain, leds light up at the top, and move downwards as if falling.
  * glitter, random lets light up and fade out.
  * rainbow, scrolling rainbow going around the table.
  * solid, single solid color.
  * all, loops between first rain, then glitter and lastly rainbow with smooth transitions inbetween.
  * snakeAI, an AI that plays snake on the table.
* /brightness?b=...
  * Set the brightness of the table, value between 0 and 255
* /color?del={true}&id=...&all={true}&new={true}&rgb={true}&r=...&g=...&b=...
  * Used to Set, Update or Delete the colors that are used in the table for rain, glitter, solid and snakeAI.
  * Use the del param to delete the color with the provided id.
    * Also possible to give the 'all' param to delete every color.
  * Use the new param to create a new color with the given r, g and b values.
  * Use only the Id param to update a color's r, g and/or b values.
    * For creating new and updating colors it's also possible to give the rgb param. This param makes it that that color cycles through the rgb spectrum.
* /speed?freq=...&fade=...&speed=...&fps=...&delta=...
  * Freq is used to determine how many leds should light up at once with the rain and glitter modes.
  * Fade is used to determine how fast a led should fade to black with the Rain (at the bottom) and glitter mode.
  * Speed is used for how fast the rainbow should move around the table.
  * Fps is the frames per second for the table. As it takes ~19ms for the data to be pushed to all the leds, the max fps is around 50.
  * Delta is how quicly the rainbow should go from color to another.
* /on
  * Turn the table on.
* /off
  * Turn the table off.
* /getColorsJsonArray
  * Get a json array of all the colors currently in use.
* /getSpeeds
  * Get a json object of the freq, fade, speed, fps and delta settings.
* /getCurrentPreset
  * Gets a json object of every setting currently in use.
  
[Link to imgur page](https://imgur.com/a/WQslLr2)
