// var oLittleImg = $('.little');
// oLittleImg.each(function(index,element){
//     var leftValue = 0;
//     if(index === 0)
//     {
//         $(element).fadeOut();
//         leftValue = 5;
//     }else{
//         $(element).fadeIn();
//         leftValue = (index+1)*5 + index*60 + 130;
//     }
//     $(element).css('left',leftValue + 'px');
//     element.index = index;
// });

// var oBigImg = $('.big');
// oBigImg.each(function(index,element){
//     if(index === 0)
//     {
//         $(element).fadeIn();
//     }else{
//         $(element).fadeOut();
//     }
//     var leftValue = (index+1)*5+index*60;
//     $(element).css('left',leftValue + 'px');
//     element.index = index;
// });

// var curFoucs = 0;
// var frontFoucs = 0;
// var animatedNum = 0;
// var hideTime = 300;
// var moveTime = 50;


// function showElement(){
//     --animatedNum;
//     if(animatedNum <= 0)
//     {
//         console.log(frontFoucs + ":" + curFoucs);

//         animatedNum = 0;

//         oBigImg.eq(frontFoucs).fadeTo(hideTime,0);
//         oLittleImg.eq(frontFoucs).fadeTo(hideTime,1);

//         oLittleImg.eq(curFoucs).fadeTo(hideTime,0);
//         oBigImg.eq(curFoucs).fadeTo(hideTime,1);
//     }    
// }


// function printPostion(object)
// {
//     object.each(function(index,element){
//         var value = $(element).position().left;
//         console.log("index:" + index + ":" + "left:" + value);
//     });
// }

// oLittleImg.on('mouseover',function(){
//     if(animatedNum !== 0)
//         return;

//     console.log("in");
//     printPostion(oLittleImg);

//     frontFoucs = curFoucs;
//     curFoucs = this.index;

//     if(frontFoucs < this.index)
//     {
//         for(let index = frontFoucs + 1; index <= curFoucs; ++index)
//         {
//             console.log("<-" + index);
//             var jQElement = oLittleImg.eq(index);
//             var eleLeft = jQElement.position().left;
//             ++animatedNum;
//             jQElement.animate({
//                 left : (eleLeft - 130) + 'px',
//             },moveTime,showElement);
//         }
//     }
//     else
//     {
//         for(let index = frontFoucs; index > curFoucs; --index)
//         {
//             console.log("->" + index);
//             var jQElement = oLittleImg.eq(index);
//             var eleLeft = jQElement.position().left;
//             console.log(eleLeft);
//             ++animatedNum;
//             jQElement.animate({
//                 left : (eleLeft + 130) + 'px',
//             },moveTime,showElement);
//         }
//     }
// });




var oLittleImg = $('.little');
oLittleImg.each(function(index,element){
    var leftValue = 0;
    if(index === 0)
    {
        $(element).fadeOut();
        leftValue = 5;
    }else{
        $(element).fadeIn();
        leftValue = (index+1)*5 + index*60 + 130;
    }
    $(element).css('left',leftValue + 'px');
    element.index = index;
});

var oBigImg = $('.big');
oBigImg.each(function(index,element){
    if(index === 0)
    {
        $(element).fadeIn();
    }else{
        $(element).fadeOut();
    }
    var leftValue = (index+1)*5+index*60;
    $(element).css('left',leftValue + 'px');
    element.index = index;
});

var curFoucs = 0;
var frontFoucs = 0;
var hideTime = 0;
var moveTime = 50;

var flag = 0;
var sum = 0;


function showElement(){
    flag = 1;

    oBigImg.eq(frontFoucs).fadeTo(hideTime,0);
    oLittleImg.eq(frontFoucs).fadeTo(hideTime,1);

    oLittleImg.eq(curFoucs).fadeTo(hideTime,0);
    oBigImg.eq(curFoucs).fadeTo(hideTime,1);  
}


// function printPostion(object)
// {
//     object.each(function(index,element){
//         var value = $(element).position().left;
//         console.log("index:" + index + ":" + "left:" + value);
//     });
// }

oLittleImg.on('mouseover',function(){ 

    oLittleImg.stop(true,true);
    oBigImg.stop(true,true);

    if(flag === 0){
        showElement();
    }else{
        flag = 0;
    }


    frontFoucs = curFoucs;
    curFoucs = this.index;

    if(frontFoucs < curFoucs)
    {
        for(let index = frontFoucs + 1; index <= curFoucs; ++index)
        {
            var jQElement = oLittleImg.eq(index);
            var eleLeft = jQElement.position().left;

            if(curFoucs === index)
            {
                jQElement.animate({
                    left : (eleLeft - 130) + 'px',
                },moveTime,showElement);
            }else{
                jQElement.animate({
                    left : (eleLeft - 130) + 'px',
                },moveTime);
            }
        }
    }
    else
    {
        for(let index = frontFoucs; index > curFoucs; --index)
        {
            var jQElement = oLittleImg.eq(index);
            var eleLeft = jQElement.position().left;

            if(frontFoucs === index)
            {
                jQElement.animate({
                    left : (eleLeft + 130) + 'px',
                },moveTime,showElement);
            }else{
                jQElement.animate({
                    left : (eleLeft + 130) + 'px',
                },moveTime,showElement);
            }
        }
    }
});










