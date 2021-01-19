$(function(){
    PainelEnv.current_interval = setInterval(function(){
        //Proibir requisiçoes simultaneas
        if (Comm.waiting){
            return;
        }
        if (Comm.isConnected){
            $("#planta-status").removeClass("bg-primary").removeClass("bg-danger").addClass("bg-success");
            $("#status-icon").removeClass("fa-times").addClass("fa-plug");
            $("#status-title").html("Conectado");
            $("#umidade-cont").show();
            $("#regar-cont").show();
            $("#conn-cont").removeClass("col-md-12");

        } else {
            $("#planta-status").removeClass("bg-primary").addClass("bg-danger").removeClass("bg-success");
            $("#status-icon").removeClass("fa-circle-o-notch").addClass("fa-times").removeClass("fa-plug");
            $("#status-title").html("Desconectado");
            $("#umidade-cont").hide();
            $("#regar-cont").hide();
            $("#conn-cont").addClass("col-md-12");
        }
        //if
        var umidade = "...";
         Comm.send("/get/umidade", function(data){
            data = data.split("umidade#")[1].split('\n')[0];
            
            if (data >= 0 && data <= 400){
                data = "Umido";
                $("#umidade-card").removeClass("bg-warning").removeClass("bg-danger").addClass("bg-info");
            }else if (data > 400 && data < 800){
                data = "Moderado";
                $("#umidade-card").addClass("bg-warning").removeClass("bg-danger").removeClass("bg-info");
            } else if (data >= 800 && data <= 1024){
                data = "Seco";
                $("#umidade-card").removeClass("bg-warning").addClass("bg-danger").removeClass("bg-info");
            }
            $('#umidade').html(data); 
        }, function(data){
            //getUmidade = 0;
        });
    }, 1000);
    

    Comm.send("/get/agenda/", function(data){
        if (parseInt(data.split("agenda#")[1].substring(0,1)) === 0){
            $("#regador-a").html("Desativado");
            $("#terr-umido").html("Desativado");
            return;
        } else {
            $("#regador-a").html("Ativado");
            $("#terr-umido").html((parseInt(data.split("agenda#")[1].substring(12,13)) === 0) ? "Não": "Sim");
        }
        for (var i = 5; i < 12; i++){
            var dia = parseInt(data.split("agenda#")[1].substring(i,i+1));
            console.log(i-4);
            if (dia === 1){
                var $tempQt = $("#day-"+(i-4));
                var html = $tempQt.html();
                $tempQt.addClass("active");
                $tempQt.html(html+" - "+ data.split("agenda#")[1].substring(1,3)+":"+ data.split("agenda#")[1].substring(3,5));
            }
        }
    });

    Comm.send("/get/qtdAgua", function(qtdAgua){
        $("#qtd-agua-info").html(qtdAgua.split("qtdAgua#")[1]+"ml");
    });


});

var irrigando = false;
$("#irrigar-btn").click(function(){
    $(this).addClass("disabled");
    if (!irrigando){
        irrigando = true;
        Comm.send("/irrigar/",function(data){      
            $("#irrigar-icon").effect("shake");      
            $("#irrigar-text").html("Preparando...");
            console.log(data.split("#")[0]);
            setTimeout(function(){
                $("#irrigar-text").html("Fazer Café"); 
                $("#irrigar-btn").removeClass('disabled'); 
                irrigando = false;
            }, data.split("#")[0]);
        }, function(){
            $(this).removeClass("disabled");
            irrigando = false;
        });

    } 
});