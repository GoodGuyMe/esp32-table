(function () {

    function sendRGB() {
        window.location.href = "/color?id=00&r=" + rOutput.innerHTML
            + "&g=" + gOutput.innerHTML
            + "&b=" + bOutput.innerHTML;
    }

    let rOutput = document.getElementById("rSliderOutput");
    let gOutput = document.getElementById("gSliderOutput");
    let bOutput = document.getElementById("bSliderOutput");

    let rSlider = document.getElementById("rSlider");
    let gSlider = document.getElementById("gSlider");
    let bSlider = document.getElementById("bSlider");

    rOutput.innerHTML = rSlider.value; // Display the default slider value
    gOutput.innerHTML = gSlider.value; // Display the default slider value
    bOutput.innerHTML = bSlider.value; // Display the default slider value

    // Update the current slider value (each time you drag the slider handle)
    rSlider.oninput = function () {
        rOutput.innerHTML = this.value;
    };
    gSlider.oninput = function () {
        gOutput.innerHTML = this.value;
    };
    bSlider.oninput = function () {
        bOutput.innerHTML = this.value;
    };

    rSlider.onmouseup = sendRGB;
    rSlider.ontouchend = sendRGB;

    gSlider.onmouseup = sendRGB;
    gSlider.ontouchend = sendRGB;

    bSlider.onmouseup = sendRGB;
    bSlider.ontouchend = sendRGB;
}) ();