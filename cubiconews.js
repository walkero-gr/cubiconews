function ml_preview()
{
	
}

jQuery(document).ready(function() {
	jQuery('#btn-preview').click(function () { 
		PrvWin = window.open("/cubiconews/preview","prv_window","toolbar=no, location=no, directories=no, status=no, menubar=no, scrollbars=yes, resizable=yes, copyhistory=no, width=725, height=550");
		PrvWin.focus();
	});
});