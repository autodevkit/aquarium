 
    //-----------------------------------------------------BOUTON 1
    //bouton 1 etat on ou off
    let r1 = document.getElementById("r1");
    let r2 = document.getElementById("r2");
    let b1 = document.getElementById("b1");

    r1.style.display = "initial"
    r2.style.display = "none"
    b1.style.display = "none";

    function onButton() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "on", true);
        xhttp.send();
        
        r2.style.display = "initial"
        r1.style.display = "none"
    }
    
    function offButton() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "off", true);
        xhttp.send();
        
        r1.style.display = "initial"
        r2.style.display = "none"
        
    }

    function ButtonMenu() {
        if(getComputedStyle(b1).display != "none"){
        b1.style.display = "none";
        } 
        else {
        b1.style.display = "block";
        }
    }
    
    //bouton 1 interval de clignotement 
    $(document).ready(function(){
        $("#appliquer").click(function(){
            var valeur = $("#choixDelayLed").val();
            $.post("delayLed",{
                valeurDelayLed: valeur
            });
        });
    });
    /*
    setInterval(function getData() {
        var xhttp = new XMLHttpRequest();
    
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                document.getElementById("valeurLuminosite").innerHTML = this.responseText;
            }
        };
    
        xhttp.open("GET", "lireLuminosite", true);
        xhttp.send();
    }, 2000);
    */
    //-----------------------------------------------------BOUTON 
    