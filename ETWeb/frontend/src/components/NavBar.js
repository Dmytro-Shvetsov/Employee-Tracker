import React, { useState } from 'react';
import * as utils from '../utils';
import {
    Button,
    Navbar,
    Nav,
    NavItem,
    Breadcrumb,
    BreadcrumbItem
} from 'reactstrap';
import { Link } from 'react-router-dom';

const MainNavBar = props => {
    const links = [
        {label: "", href: "/", id:"logo"},
        {label: "Features", href: "/features"},
        {label: "How it works", href: "/idea"},
        {label: "Benefits", href: "/benefits"},
        {label: "Contact Us", href: "/contact"},
    ];

    const authItems = [];
    if (props.user !== undefined && props.user !== null) {
        authItems.push({label: "Profile", href: "/dashboard"}, {label: "Log Out", href: "/logout"})
    } else {
        authItems.push({label: "Log In", href: "/login"}, {label: "Sign Up", href: "/register"})
    }

    let [activeItem, setActiveItem] = useState(utils.getActiveNavItemIdx(links.concat(authItems)) || -1);
    return (
        <Navbar color="dark" expand="md">
            <Nav className="m-auto" id="navbar" navbar>
                {links.map((item, idx) => {
                    const {label, href, ...rest} = item;
                    return (
                        <NavItem
                            onClick={() => setActiveItem(idx)}
                            key={idx}
                            active={activeItem === idx}
                            {...rest}
                        >
                            <Link to={href} className="nav-link text-light">{label}</Link>
                        </NavItem>
                    );
                })}

                <div id="auth-items">
                    {authItems.map((item, idx) => {
                        const {label, href, ...rest} = item;
                        const shiftedIdx = idx + links.length;
                        return (
                            <Button
                                color="light"
                                onClick={() => setActiveItem(shiftedIdx)}
                                key={shiftedIdx}
                                active={activeItem === shiftedIdx}
                                {...rest}
                            >
                                <Link className="text-dark nav-link" to={href}>{label}</Link>
                            </Button>
                        );
                    })}
                </div>
            </Nav>
        </Navbar>
    );
};

const BreadCrumb = props => {
    const { breadcrumbItemsList } = props;
    const lastItemIdx = breadcrumbItemsList.length - 1;

    return (
        <div>
          <Breadcrumb tag="nav" listTag="div">
              {breadcrumbItemsList.map(
                  (item, idx) => (
                      <BreadcrumbItem
                          tag={props => (
                                    <Link to={props.href}
                                          className={`breadcrumb-item${idx === lastItemIdx ? " active" : ""}`}
                                    >
                                        {props.children}
                                    </Link>)}
                          href={item.href}
                          key={idx}
                      >
                          {item.text}
                      </BreadcrumbItem>
                  )
              )}
          </Breadcrumb>
        </div>
    );
};

export default MainNavBar;
export {
    MainNavBar as NavBar,
    BreadCrumb
};