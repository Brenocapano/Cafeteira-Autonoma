const PainelEnv = {
    page_loader_tag: null,
    current_page: "index",
    current_interval: null,
    getPage: function(){
        if (PainelEnv.page_loader_tag == null){
            return;
        }
        var url_parts = window.location.href.split("#/");
        if (url_parts.length == 1){
            PainelEnv.current_page = "index";
            window.location.href += "#/index";
        }
        PainelEnv.page_loader_tag.html('<h1 class="text-center"><i class="fa fa-circle-o-notch fa-spin fa-3x fa-fw"></i></h1><hr><p class="text-center">Carregando...</p>');
        $.ajax({
            method: "GET",
            url: "assets/pages/" + url_parts[1] + ".html",
            success: function(data){
                PainelEnv.current_page = url_parts[1];
               // console.log(PainelEnv.current_page);
                clearInterval(PainelEnv.current_interval);
                PainelEnv.current_interval = null;
                PainelEnv.page_loader_tag.html(data);
                PainelEnv.setMenu();
            },
            error: function(){
                $("#main-menu .list-group-item").removeClass("active");
                PainelEnv.page_loader_tag.html('<h1 class="text-center">404</h1><hr><p class="text-center">Página não encontrada.</p>');
            }
        });
    },

    setMenu: function(){
        $("#main-menu .list-group-item").removeClass("active");
        $("#main-menu .list-group-item").each(function(i, elem){
            var page = elem.href.split("/#/")[1];
            if (page === PainelEnv.current_page){
                $(elem).addClass("active");
                return;
            }
        });
    },

    init: function(page_loader_tag){
        $(window).bind('hashchange', function() {
            PainelEnv.getPage();
        });
        PainelEnv.page_loader_tag = $(page_loader_tag);
        PainelEnv.getPage();
    }

}