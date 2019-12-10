var oLis = $('li');
oLis.eq(0).css('backgroundColor', 'red');

var content = $('.content');
content.text('内容1');

oLis.each(function(index,element){
    element.index = index+1;
});

oLis.click(function(){
    oLis.each(function(index,element){
        $(element).css('backgroundColor', 'white');
    });
    content.text("内容" + this.index);

    $(this).css('backgroundColor', 'red');
});