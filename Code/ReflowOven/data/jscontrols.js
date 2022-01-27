
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

var Profiles = ['Solder Paste 1', 'Solder Paste 2', 'Preheat','new']


var ReflowChart = null;
var PreheatTemp = 0;
var PreheatRamp = 0;
var PreheatDwel = 0;
var FlowTemp = 0;
var FlowDwel = 0;
var FlowRamp = 0;
var CoolRamp = 0;
var Verson = 0;
var CurrentTemp = 0;
var Connected = 0;

function SetRelowGraph(){
    PreheatTemp = 125;
    PreheatRamp = 5;
    PreheatDwel = 5;
    FlowTemp = 286;
    FlowDwel = 5;
    FlowRamp = 7;
    CoolRamp = 8;
    document.getElementById('ProfileName').value = Profiles[0];
    document.getElementById('preheatTemp').value = PreheatTemp;
    document.getElementById('preheatRamp').value = PreheatRamp;
    document.getElementById('preheatDwel').value = PreheatDwel;
    document.getElementById('flowTemp').value = FlowTemp;
    document.getElementById('flowDwel').value= FlowDwel;
    document.getElementById('flowRamp').value = FlowRamp;
    document.getElementById('coolRamp').value = CoolRamp;
    var newtemp = new Array(0, parseInt(PreheatTemp), parseInt(PreheatTemp),parseInt(FlowTemp),parseInt(FlowTemp),0);
    ReflowChart = Highcharts.chart('chart-temperature', {
        title: {
            text: 'Reflow Profile'
        },
        xAxis: {
            title: {
                text: 'Timeline'
            },
            max: 8
        },
        yAxis: {
            title: {
                text: 'Temprature (°C)'
            },
        },
        series: [{
          data: newtemp
        }]
    });
}

function changePageTitle(mode,present) {
    if(mode == 1){
        const Preamble = 'Reflowing ';
        const End = '%';
        document.title = '${Preamble}%{present}${End}';
    }
    if(mode == 2){
        const Preamble = 'Preheating ';
        const End = '%';
        document.title = '${Preamble}%{present}${End}';
    }
}

function LoadCBProfile() {
    var select = document.getElementById("comboProfile");
    for (var i = 0; i < Profiles.length; i++) {
        var optn = Profiles[i];
        var el = document.createElement("option");
        el.textContent = optn;
        el.value = optn;
        select.appendChild(el);
    }
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
    Connected = 1;
    VersonInfo();
    GetProfileNames();
    LoadCBProfile();
}

function onClose(event) {
    console.log('Connection closed');
    Connected = 0;
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    let data = JSON.parse(event.data);
    console.log(data);
    if(data.hasOwnProperty('VER')){
        Verson = data["VER"];
    }
    if(data.hasOwnProperty('CNTTMP')){
        CurrentTemp = data["CNTTMP"];
    }
    if(data.hasOwnProperty('RESP')){
        if(data["RESP"] == 1){
            window.setTimeout('alert("Oven Started");window.close();', 2500);
        }
        else{
            window.setTimeout('alert("Oven Stoped");window.close();', 2500);
        }
    }
    if(data.hasOwnProperty('RESP')){
        if(data["RESP"] == 1){
            window.setTimeout('alert("Oven Started");window.close();', 2500);
        }
        else{
            window.setTimeout('alert("Oven Stoped");window.close();', 2500);
        }
    }    
}

function WiFiSendInfo(){
    var ssid = document.getElementById('SSID').value
    var pswd = document.getElementById('WiFiPswd').value
    websocket.send(JSON.stringify({'TYP':1,'WiFiSSID':ssid,'WiFiPSWD':pswd}));
}

function GetProfileInfo(Profile){
    websocket.send(JSON.stringify({'TYP':2,'Profile':Profile}));
}

const interval = setInterval(function() {
    if(Connected){
        websocket.send(JSON.stringify({'TYP':3}));
    }
}, 2000);

function VersonInfo(){
    websocket.send(JSON.stringify({'TYP':4}));
}

function Start(Profile){
    websocket.send(JSON.stringify({'TYP':5,'Profile':Profile}));
}


function GetProfileNames(){
    websocket.send(JSON.stringify({'TYP':6}));
}



function ReflowSendInfo(){
    ProfileName = document.getElementById('ProfileName').value;
    PreheatTemp = document.getElementById('preheatTemp').value;
    PreheatRamp = document.getElementById('preheatRamp').value;
    PreheatDwel = document.getElementById('preheatDwel').value;
    FlowTemp = document.getElementById('flowTemp').value;
    FlowDwel = document.getElementById('flowDwel').value;
    FlowRamp = document.getElementById('flowRamp').value;
    CoolRamp = document.getElementById('coolRamp').value;
    websocket.send(JSON.stringify({'TYP':2,'PNM':ProfileName,'PHT':PreheatTemp,'PHR':PreheatRamp,'PHR':PreheatDwel,'FLT':FlowTemp,'FLD':FlowDwel,'FLR':FlowRamp,'CDR':CoolRamp}));
}

function GetTemp(){
    return CurrentTemp;
}

function GetVerson(){
    return Verson;
}

function GetConnectionStat(){
    return Connected;
}