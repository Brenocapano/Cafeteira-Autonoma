
$(function(){
    
    Comm.send("/get/agenda/", function(data){
        data = data.split("agenda#")[1];
        if (parseInt(data.substr(0,1)) === 1){
            $("#is-actived").prop('checked', true);
            $("#activated-area").fadeIn();
            var hora = data.substring(1,3);
            var min = data.substring(3,5);
            $("#hora").val(hora+":"+min);
            for (var i = 1; i <= 7;i++){
                $("#day-"+i).prop('checked', (data.substring(4+i,4+i+1) === "1")? true: false);
            }
            if (data.substring(12,13) === "1"){
                $("#protect-plant").prop('checked', true);
            } else {
                $("#protect-plant").prop('checked', false);
            }
            return;
        } else {
            $("#activated-area").fadeOut();
        }
    })

});

$("#is-actived").click(function(){
    if ($(this).is(":checked")){
        $("#activated-area").slideDown();
    } else {
        $("#activated-area").fadeOut();
    }
});

$("#agenda-form").submit(function(){
    var $this = $("#submit-button");
    var temp = $($this).html();
    $($this).html("Salvando...").attr('disabled','disabled');
    var builder = "";
    if ($("#is-actived").is(":checked")){
        builder += "1";
    } else {
        builder += "0";
    }
    var  horaV = $("#hora").val().split(":");
    builder += (horaV[0] === undefined) ? "00": ""+horaV[0];
    builder += (horaV[1] === undefined) ? "00": ""+horaV[1];
    for (var i = 5; i <= 11; i++){
        if ($("#day-"+(i-4)).is(":checked")){
            builder += "1";
        }else {
            builder += "0";
        }
    }
    if ($("#protect-plant").is(":checked")){
        builder += "1";
    } else {
        builder += "0";
    }
    Comm.send("/set/agenda/?"+builder.trim(), function(data){
        $this.html(temp).removeAttr("disabled");
        $("#aviso").show().html("O agendamento foi salvo com sucesso!");
    }, function(){
        $this.html(temp).removeAttr("disabled");
    });
    return false;
});