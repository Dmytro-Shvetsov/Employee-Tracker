import React, { useState } from "react";
import { ListGroup, ListGroupItem } from "reactstrap";
import { Link, useRouteMatch} from "react-router-dom";
import * as utils from "../../utils";

const DashboardNavBar = props => {
    const routeMatch = useRouteMatch();
    const links = [
        {label: "Profile", href: "/profile"},
        {label: "Account", href: "/account"},
    ];

    if (props.user && props.user.is_staff) {
        links.push({label: "My Projects", href: "/projects"})
    }
    // add some more items to make navbar more extensive (will change in future)
    links.push(
            {label: "item4", href: "/whatever4"},
            {label: "item5", href: "/whatever5"}
        );

    let [activeItem, setActiveItem] = useState(utils.getActiveNavItemIdx(links) || 0);

    return (
        <ListGroup>
            {links.map((item, idx) => (
                <ListGroupItem
                    action
                    onClick={() => setActiveItem(idx)}
                    key={idx}
                    active={activeItem === idx}
                >
                    <Link to={routeMatch.url + item.href} className="nav-link text-dark">
                        {item.label}
                    </Link>
                </ListGroupItem>
            ))}
        </ListGroup>
    );
};

export default DashboardNavBar;