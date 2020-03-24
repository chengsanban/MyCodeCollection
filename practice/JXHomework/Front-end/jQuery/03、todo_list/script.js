let oInput = $('input');
let oDoingList = $('.doing');
let oDoneList = $('.done');
let oDoingListNum = $('.doingTitle .number');
oDoingListNum[0].number = 0;
let oDoneListNum = $('.doneTitle .number');
oDoneListNum[0].number = 0;

oInput.keydown(function(event){
    if(event.keyCode === 13)
    {
        if(oInput.val() === "")
            return;
        genDoingList();
    }
});

function genDoingList()
{
    ++oDoingListNum[0].number;
    oDoingListNum.text(oDoingListNum[0].number);

    var oList = $('<div></div>');
    oList.addClass('list');

    var oCheck = $('<input type="checkbox" />');
    oList.append(oCheck);
    oCheck.click(function(){
        var oTempList = $(this).parent();
        if($(this).prop("checked"))
        {
            oTempList.css('opacity','0.5');
            oTempList.clone(true).appendTo(oDoneList);
            oTempList.remove(); 
    
            --oDoingListNum[0].number;
            oDoingListNum.text(oDoingListNum[0].number);
            ++oDoneListNum[0].number;
            oDoneListNum.text(oDoneListNum[0].number);
        }else{
            oTempList.css('opacity','1');
            oTempList.clone(true).appendTo(oDoingList);
            oTempList.remove(); 
    
            ++oDoingListNum[0].number;
            oDoingListNum.text(oDoingListNum[0].number);
            --oDoneListNum[0].number;
            oDoneListNum.text(oDoneListNum[0].number);
        }
    });

    var oListName = $('<div></div>');
    oListName.text(oInput.val());
    oInput.val("");
    oListName.addClass('listName');
    oList.append(oListName);

    var oDeleteButton = $('<div>X</div>');
    oDeleteButton.addClass('deleteButton');
    oDeleteButton.click(function(){
        var temp = $(this).parent().parent();
        if(temp.hasClass('done'))
        {
            --oDoneListNum[0].number;
            oDoneListNum.text(oDoneListNum[0].number);
        }else{
            --oDoingListNum[0].number;
            oDoingListNum.text(oDoingListNum[0].number);
        }
        $(this).parent().remove(); 
    });
    oList.append(oDeleteButton);

    oDoingList.append(oList);
}
