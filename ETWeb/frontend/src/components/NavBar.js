import React from 'react';
import {
    Button,
    Navbar,
    Nav,
    NavItem,
    ListGroup,
    ListGroupItem,
    Breadcrumb,
    BreadcrumbItem
} from 'reactstrap';
import { Link, useRouteMatch } from 'react-router-dom';
import { userLoggedIn } from '../services/authService'


const AuthItems = props => {
    const { user } = props;

    if (userLoggedIn(user)) {
        return (
            <React.Fragment>
                <Button color="light" key="profile" className="auth-btn">
                    <Link className="text-dark nav-link" to="/dashboard">Profile</Link>
                </Button>
                <Button color="light" key="logout" className="auth-btn">
                    <Link className="text-dark nav-link" to="/logout">Log Out</Link>
                </Button>
            </React.Fragment>
        );
    }
    return (
        <React.Fragment>
            <Button color="light" key="login" className="auth-btn">
                <Link className="text-dark nav-link" to="/login">Log In</Link>
            </Button>
            <Button color="light" key="signup" className="auth-btn">
                <Link className="text-dark nav-link" to="/register">Sign Up</Link>
            </Button>
        </React.Fragment>
    );
};

class MainNavBar extends React.Component {
    constructor(props) {
        super(props);
    }

    render() {
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
                        <AuthItems user={this.props.user}/>
                    </div>
                </Nav>
            </Navbar>
        );
    }
}

const DashboardNavBar = props => {
    const routeMatch = useRouteMatch();
    const { user } = props;

    return (
        <ListGroup>
            <ListGroupItem>
                <Link to={`${routeMatch.url}/profile`} className="nav-link text-dark">Account</Link>
            </ListGroupItem>
            {user.is_staff && (
                <ListGroupItem>
                    <Link to={`${routeMatch.url}/projects`} className="nav-link text-dark">My Projects</Link>
                </ListGroupItem>
            )}
            <ListGroupItem>
                <Link to={`${routeMatch.url}/ads`} className="nav-link text-dark">Item3</Link>
            </ListGroupItem>
            <ListGroupItem>
                <Link to={`${routeMatch.url}/asd`} className="nav-link text-dark">Item4</Link>
            </ListGroupItem>
            <ListGroupItem>
                <Link to={`${routeMatch.url}/asd`} className="nav-link text-dark">Item5</Link>
            </ListGroupItem>
        </ListGroup>
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
    DashboardNavBar,
    BreadCrumb
};