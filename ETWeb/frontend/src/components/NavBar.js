import React, { useState } from 'react';
import * as utils from "../utils";
import {
    Button,
    Navbar,
    Nav,
    NavItem,
    Breadcrumb,
    BreadcrumbItem, ListGroupItem
} from 'reactstrap';
import { Link } from 'react-router-dom';
import { userLoggedIn } from '../services/authService'



const AuthItems = props => {
    const { user } = props;

    if (userLoggedIn(user)) {
        return (
            <React.Fragment>
                <Button color="light" key="profile">
                    <Link className="text-dark nav-link" to="/dashboard">Profile</Link>
                </Button>
                <Button color="light" key="logout">
                    <Link className="text-dark nav-link" to="/logout">Log Out</Link>
                </Button>
            </React.Fragment>
        );
    }
    return (
        <React.Fragment>
            <Button color="light" key="login">
                <Link className="text-dark nav-link" to="/login">Log In</Link>
            </Button>
            <Button color="light" key="signup">
                <Link className="text-dark nav-link" to="/register">Sign Up</Link>
            </Button>
        </React.Fragment>
    );
};

const MainNavBar = props => {
    const links = [
        {label: "Here will be logo", href: "/", id:"logo"},
        {label: "Features", href: "/features"},
        {label: "How it works", href: "/idea"},
        {label: "Benefits", href: "/benefits"},
        {label: "Contact Us", href: "/contact"},
    ];

    const authItems = [];
    if (userLoggedIn(props.user)) {
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