import React from 'react';
import {
    Button,
    Navbar,
    Nav,
    NavItem,
    Breadcrumb,
    BreadcrumbItem
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
    return (
        <Navbar color="dark" expand="md">
            <Nav className="m-auto" id="navbar" navbar>
                <NavItem>
                    <Link to="/" id="logo" className="nav-link text-light">Employee Tracker</Link>
                </NavItem>
                <NavItem active key="features">
                    <Link to="/features" className="nav-link text-light">Features</Link>
                </NavItem>
                <NavItem>
                    <Link to="/idea" className="nav-link text-light">How it works</Link>
                </NavItem>
                <NavItem>
                    <Link to="/benefits" className="nav-link text-light">Benefits</Link>
                </NavItem>
                <NavItem>
                    <Link to="/contact" className="nav-link text-light">Contact Us</Link>
                </NavItem>
                <div id="auth-items">
                    <AuthItems user={props.user}/>
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