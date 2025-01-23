
async function handler(event) {
    event.preventDefault();
    element = event.target;
        let xtype = element.getAttribute('hl-req');
        if (xtype === null) xtype = "post";
        let xurl = element.getAttribute('hl-url');
        let xtrigger = element.getAttribute('hl-trigger');
        let xtarget = element.getAttribute('hl-target');
        let xswap = element.getAttribute('hl-swap');

        if (!xswap) {
            xswap = 'innerHTML';
        }

        let eltarget;
        if (xtarget === null) {
          eltarget = element;
        } else {
          eltarget = element.closest(xtarget);
        }

    let data;
    let xdata = {};
    if (event.currentTarget.tagName.toLowerCase() === 'form') {
        data = new FormData(event.currentTarget);
    } else {
        xdata["evtype"] = event.type
	data = "evtype="+event.type
        let names;
        if (xtarget === null) {
          names = [element];
        } else {
          names = eltarget.querySelectorAll(":scope [name]");
        }
        for (let i = 0; i < names.length; i++) {
           let item = names[i];
           let name = item.getAttribute("name")
           let value = item.value
           data = data +"&"+name+"="+value
           xdata[name] = value;
        }

    }

    response = await fetch(xurl, {
        method: xtype,
        headers: {
           'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: new URLSearchParams(xdata)
    })
    if (response.status == 200 && xswap != 'none' && xtype != 'load') {
       content = await response.text()

            let adjacent = (xswap != 'innerHTML' && xswap != 'outerHTML');

            if (xswap == 'delete') {
                target.parentNode.removeChild(target);
                return;
            }

            if (adjacent) {
                //console.log('fill adjacent',xswap,eltarget,eltarget[xswap])
                eltarget.insertAdjacentHTML(xswap, content);
            } else {
                eltarget[xswap] = content;
                //console.log('fill non-adjacent',xswap,eltarget)
                let elements = target.querySelectorAll(':scope [hl-url]');
                setup(elements);
            }
    }

}

document.addEventListener('DOMContentLoaded', () => {
    let elements = document.querySelectorAll('[hl-url]');
    setup(elements);
});

function setup(elements) {
    //console.log("elements",elements)
    for (let i = 0; i < elements.length; i++) {
        let element = elements[i];

        let trigger = element.getAttribute('hl-trigger');

        if (!trigger) {
            switch (element.tagName.toLowerCase()) {
                case 'input':
                case 'textarea':
                case 'select':
                    trigger = 'change';
                    break;
                case 'form':
                    trigger = 'submit';
                    break;
                default:
                    trigger = 'click';
            }
        }

        let triggers = trigger.split(",");
        for (let j = 0; j < triggers.length; j++) {
            element.addEventListener(triggers[j].trim(), handler);

            if (triggers[j].trim() == 'load') {
                element.dispatchEvent(new Event('load'));
            }
        }
    }
}
