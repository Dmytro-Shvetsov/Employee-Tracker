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

/**
 * Make shallow copy of Set object.
 * @returns {Set<any>} New instance with copied elements.
 */
Set.prototype.copy = function () {
    console.log(this.entries());
    const newInstance = new Set();
    this.forEach(item => {
        newInstance.add(item);
    });
    return newInstance;
};

/**
 * Analogous array's map function for Set object.
 * @param callback {function} Function to apply to elements
 * @returns {Set<any>} New array with transformed elements.
 */
Set.prototype.map = function (callback) {
    const newArray = [];
    this.forEach(item => {
        newArray.push(callback(item));
    });
    return newArray;
};