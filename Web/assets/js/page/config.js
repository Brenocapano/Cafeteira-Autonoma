var stopclook = false;
var aguaGet = false;

$(function(){
    $("#addr-input").val(Data.get("ip"));
    $("#ml-input-config").attr("placeholder","Carregando...");
    loadQtdAgua();
});


PainelEnv.current_interval = setInterval(function(){
    if (Comm.isConnected){
        $("#is-con, #ajusta-hora-btn, #ml-input-config").show().removeAttr("disabled", "disabled");
    } else {
        $("#is-con, #ajusta-hora-btn, #ml-input-config").hide().attr("disabled", "disabled");
    }
    
}, 1000);


function encodeWeek(oldWeek){
    switch(parseInt(oldWeek)){
        case 7: return 1; //Domingo
        case 1: return 2; //Segunda
        case 2: return 3; //terça
        case 3: return 4; //Quarta
        case 4: return 5; //QUinta
        case 5: return 6; //Sexta
        case 6: return 7; //Sabado
    }
}


// function getHora(cbk){
//     if (stopclook) {return}
//     Comm.send("/get/hora/", function(data){
//         data = data.split("hora#")[1].split(";");
//         data = data[3]+"-"+data[2]+"-"+data[1]+" "+data[4]+":"+data[5]+":"+data[6];
//         data = moment(data).format("LLLL\:s");
//         $("#hora").html(data);
//         if (typeof cbk === 'function'){
//             cbk();
//         }
//     }, function(){
//         $("#hora").html("...");
//     });
// }


function encodeNowTime(){
    var time = encodeWeek(moment().format("E")); 
    if (moment().format("D") <= 9){
        time += "0";
    }
    time += moment().format("D");
    if (moment().format("M") <= 9){
        time += "0";
    }
    time += moment().format("M");
    time += moment().format("Y");
    if (moment().format("H") <= 9){
        time += "0";
    }
    time += moment().format("H");
    if (moment().format("m") <= 9){
        time += "0";
    }
    time += moment().format("m");
    if (moment().format("s") <= 9){
        time += "0";
    }
    time += moment().format("s");
    return time;
}

function loadQtdAgua(){
    Comm.send("/get/qtdAgua/", function(qtd){
        $("#ml-input-config").removeAttr("placeholder").removeAttr("disabled").val(parseInt(qtd.split("qtdAgua#")[1])); 
        //console.log(qtd);
        aguaGet = true;
    });
}

$("#salvar-btn").click(function(){
    stopclook = true;
    Data.inputOrUpdate("ip",$("#addr-input").val());
    Comm.updateIp(Data.get("ip"));
    $("#aviso").show().html(" * Configurações de IP salva.");
    var qtdAgua = $("#ml-input-config").val();
    while (qtdAgua.length < 5){
        qtdAgua +="#";
    }
    Comm.sendForce("/set/qtdAgua/",qtdAgua, function(){
        $("#aviso").show().append("<br> * Configurações de quantidade de água salva.");
        stopclook = false;
    });
});


$("#ajusta-hora-btn").click(function(){
    var temp = $(this).html()
    var $this = $(this);
    $(this).html("Enviano...").attr('disabled','disabled');
    Comm.send("/set/hora/?"+encodeNowTime(), function(res){
        $this.html(temp).removeAttr("disabled");
        $("#aviso").show().html("Hora ajustada.");
    }, function(){
        $this.html(temp).removeAttr("disabled");
    });
});