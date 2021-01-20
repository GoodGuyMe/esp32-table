(function () {

    function sendFreq() {
        window.location.href = "/speed?freq=" + freqOutput.innerHTML;
    }

    function sendFade() {
        window.location.href = "/speed?fade=" + fadeOutput.innerHTML;
    }

    function sendSpeed() {
        window.location.href = "/speed?speed=" + speedOutput.innerHTML;
    }

    function sendFPS() {
        window.location.href = "/speed?fps=" + fpsOutput.innerHTML;
    }

    let freqOutput = document.getElementById("freqSliderOutput");
    let fadeOutput = document.getElementById("fadeSliderOutput");
    let speedOutput = document.getElementById("speedSliderOutput");
    let fpsOutput = document.getElementById("fpsSliderOutput");

    let freqSlider = document.getElementById("freqSlider");
    let fadeSlider = document.getElementById("fadeSlider");
    let speedSlider = document.getElementById("speedSlider");
    let fpsSlider = document.getElementById("fpsSlider");

    freqOutput.innerHTML = freqSlider.value; // Display the default slider value
    fadeOutput.innerHTML = fadeSlider.value; // Display the default slider value
    speedOutput.innerHTML = speedSlider.value; // Display the default slider value
    fpsOutput.innerHTML = fpsSlider.value; // Display the default slider value

    // Update the current slider value (each time you drag the slider handle)
    freqSlider.oninput = function () {
        freqOutput.innerHTML = this.value;
    };
    fadeSlider.oninput = function () {
        fadeOutput.innerHTML = this.value;
    };
    speedSlider.oninput = function () {
        speedOutput.innerHTML = this.value;
    };
    fpsSlider.oninput = function () {
        fpsOutput.innerHTML = this.value;
    };

    freqSlider.onmouseup = sendFreq;
    freqSlider.ontouchend = sendFreq;

    fadeSlider.onmouseup = sendFade;
    fadeSlider.ontouchend = sendFade;

    speedSlider.onmouseup = sendSpeed;
    speedSlider.ontouchend = sendSpeed;

    fpsSlider.onmouseup = sendFPS;
    fpsSlider.ontouchend = sendFPS;
}) ();