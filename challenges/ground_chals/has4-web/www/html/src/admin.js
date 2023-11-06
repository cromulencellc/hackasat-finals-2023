
import { requestServerConfig, requestServerDashboard } from './admin_api.js';

var dashboard_timer;

export async function setsystemstatus() {

    var result = await requestServerDashboard();

    var jsonData = JSON.parse(result);

    var idle = jsonData['Overall CPU %Idle'];

    console.log(idle);

    var busy = (100 - idle)/2;

    console.log(busy);

    var display = `Aggregate CPU Load<div style="width:50%;background-color:gray;"><div style="height:24px;width:${busy}%;background-color:red;"></div></div><br>`;

    let cpucores = jsonData['Cores %Idle'];

    console.log("len = " + Object.keys(cpucores).length);

    for (let i=0; i < Object.keys(cpucores).length; ++i) {

        console.log(cpucores[i]);
        var key;
        for (key in cpucores[i]) {

            var value = cpucores[i][key];
            var busy = (100 - value)/2;

            display += `${key}<div style="width:50%;background-color:gray;"><div style="height:24px;width:${busy}%;background-color:red;"></div></div><br>`;
        }

    }

    var totalMem = Math.round(jsonData['Total Mem']/1000000);
    var freeMem = Math.round(jsonData['Free Mem']/1000000);

    var usedMemPercent = (totalMem - freeMem)*100/totalMem;

    display += `<p>Total Memory ${totalMem}MB</p>`
    display += `Used Memory<div style="width:50%;background-color:gray;"><div style="height:24px;width:${usedMemPercent}%;background-color:red;"></div></div><br>`;

    document.getElementById("response").innerHTML = display;

    dashboard_timer = setTimeout(setsystemstatus, 2000);           

}

export async function setsystemconfig() {

    clearInterval(dashboard_timer);

    var result = await requestServerConfig();

    var jsonData = JSON.parse(result);

    var table = "<table>";

    for (var key in jsonData) {

        console.log(key + ": " + jsonData[key]);
        table+= "<tr><td>" + key + "</td><td>" + jsonData[key] + "</td></tr>"
    }

    table+= "</table>"
    document.getElementById("response").innerHTML = table;

}

var dashboard_timer;

setsystemstatus();