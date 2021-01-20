(function () {

    function sendBrightness() {
        window.location.href = "/brightness?b=" + brightOutput.innerHTML;
    }

    let brightOutput = document.getElementById("brightSliderOutput");

    let brightSlider = document.getElementById("brightSlider");

    brightOutput.innerHTML = brightSlider.value; // Display the default slider value

    // Update the current slider value (each time you drag the slider handle)
    brightSlider.oninput = function () {
        brightOutput.innerHTML = this.value;
    };

    brightSlider.onmouseup = sendBrightness;
    brightSlider.ontouchend = sendBrightness;
}) ();