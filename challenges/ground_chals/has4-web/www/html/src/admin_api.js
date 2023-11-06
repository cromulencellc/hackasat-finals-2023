

export async function requestServerDashboard() {

    const message = JSON.stringify({'dashboard':'get'});

    const result =  await sendPostRequest('/!admin/dashboard', message);

    console.log(result);

    return result;
}

export async function requestServerConfig() {

    const message = JSON.stringify({'config':'get'});
    const result = await sendPostRequest('/!admin/config', message);

    return result;
}

export async function requestServerFile(path) {

    const message = JSON.stringify({'file':'get',
                                    'path': path
                });

    const result = await sendPostRequest('/!admin/files', message);

    return result;
}

export async function requestServerFilelist(path) {

    const message = JSON.stringify({'file':'list',
                                    'path' : path
                });

    const result = await sendPostRequest('/!admin/files', message);

    return result;
}

async function sendPostRequest(url, data) {

    var response = fetch(url, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: data
    })
    .then((response) => response.body)
    .then((rb) => {
        const reader = rb.getReader();

            return new ReadableStream({
                start(controller) {
                // The following function handles each data chunk
                function push() {
                // "done" is a Boolean and value a "Uint8Array"
                reader.read().then(({ done, value }) => {
                    // If there is no more data to read
                    if (done) {
                    controller.close();
                    return;
                    }
                    // Get the data and send it to the browser via the controller
                    controller.enqueue(value);

                    push();
                });
                }
                push();
            },
            });
        })
        .then((stream) =>
            // Respond with our stream
            new Response(stream, { headers: { "Content-Type": "application/json" } }).text(),
        )
        .then((result) => {
            // Do things with result
            return result;

        });

        return response;
}

function makeWindow(name,height,width) {

    var top = (screen.height-height)/2;

    if (top < 0) {
        top = 0; height = screen.availHeight;
    }

    var left = (screen.width-width)/2;

    if (left < 0) {
        left = 0; width = screen.availWidth;
    }
    
    return window.open('',name,'resizeable=yes,scrollbars=yes,height='+height+',width='+width+',top='+top+',left='+left);
}