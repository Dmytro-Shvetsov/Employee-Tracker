export function typeOf(obj) {
    return ({}).toString.call(obj).match(/\s(\w+)/)[1].toLowerCase();
}

export function checkTypes(args, types) {
    args = [].slice.call(args);

    for (let i = 0; i < types.length; i++) {
        if (typeOf(args[i]) !== types[i]) {
            throw new TypeError('param ' + i + ' must be of type ' + types[i]);
        }
    }
}

export function capitalize(str) {
    checkTypes(arguments, ['string']);
    return `${str.charAt(0).toUpperCase()}${str.slice(1)}`
}

/**
 * Get active link by parsing current location path.
 * @param links list of objects, which must have href property
 * @returns {int} index of
 */
export function getActiveNavItemIdx(links) {
    const lastPathPart = "/" + location.pathname.split("/").pop(-1);
    const pattern = new RegExp(lastPathPart, 'i');
    let initActiveItem = null;
    links.forEach((item, idx) => {
        if (item.href.match(pattern)) {
            initActiveItem = idx;
        }
    });
    return initActiveItem;
}