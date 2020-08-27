import React from 'react';
import axios from 'axios';
import * as auth from '../../services/authService';
import { Button, Form, FormGroup, Label, Alert, Container } from 'reactstrap';
import { Link, Redirect } from 'react-router-dom';
import {Input} from '../common/index'


export default class LoginForm extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            data: {
                username: '',
                password: '',
            },
            rememberMe: false,
            errors: {},
        };
        this.reqSource = undefined;
        this._isMounted = false;
    }

    setState = (...args) => {
        if (this._isMounted) {
            super.setState(...args);
        }
    };

    handleInputChange = event => {
        const target = event.target;
        const name = target.name;

        this.setState({
            data: { ...this.state.data, [name]: target.value },
            errors: { ...this.state.errors, [name]: undefined },
        });
    };
    handleRememberMeChange = event => {
        this.setState({
            rememberMe: !this.state.rememberMe,
        });
    };

    handleSubmit = async event => {
        event.preventDefault();
        await this.cancelPreviousRequests();
        const { data, rememberMe:remember } = this.state;
        try {
            const response = await auth.loginUser({...data, remember}, this.reqSource.token);
            this.setState({
                errors: {},
                user: response.data
            });
            this.props.onLogin(response.data);
        } catch (error) {
            console.log(error.message);
            if (error.response.status === 400) {
                const fieldErrors = error.response.data;
                Object.keys(fieldErrors).map((fieldName) => {
                    fieldErrors[fieldName] = fieldErrors[fieldName].join(" ");
                    // console.log(fieldErrors[fieldName]);
                });

                this.setState({
                    errors: fieldErrors
                });
            }
        }
    };

    cancelPreviousRequests = async () => {
        if (this.reqSource) {
            this.reqSource.cancel();
        }
        this.reqSource = axios.CancelToken.source();
    };

    componentDidMount() {
        this._isMounted = true;
    }

    async componentWillUnmount() {
        this._isMounted = false;
        await this.cancelPreviousRequests();
    }

    render() {
        const { user, errors } = this.state;

        if (user !== undefined) {
            if (this.props.location && this.props.location.state) {
                const {state} = this.props.location;
                return <Redirect to={{
                    pathname: state.next || "/",
                    state: {user},
                }}/>
            }
            return <Redirect to="/"/>
        }

        return (
            <React.Fragment>
                <Container className="col-8" style={{margin: "auto"}}>
                    <h1>Account Log In</h1>
                    <p>
                        Use the form below to log in to your online account.
                        If you are having trouble please use the 'forgot username/password'
                        link or <Link to = "/contact">contact our staff</Link> for assistance.
                    </p>
                </Container>
                <Form className="form-center" autoComplete="on">
                    <Input
                        name="username"
                        autoComplete="username"
                        labelText="Username"
                        error={errors.username}
                        onChange={this.handleInputChange}
                    />
                    <Input
                        name="password"
                        autoComplete="current-password"
                        labelText="Password"
                        error={errors.password}
                        onChange={this.handleInputChange}
                        type="password"
                    />
                    <Input
                        name="rememberMe"
                        labelText="Remember me"
                        labelFirst={false}
                        type="checkbox"
                        onChange={this.handleRememberMeChange}
                    />
                    <FormGroup>
                        <Alert color="danger" isOpen={errors.non_field_errors !== undefined}>
                            {errors.non_field_errors}
                        </Alert>
                    </FormGroup>
                    <FormGroup className="auth-other">
                        <Label>
                            <Link to="/reset-password">Forgot your username or password?</Link>
                        </Label>
                        <br />
                        <Button onClick={this.handleSubmit}>Submit</Button>
                    </FormGroup>
                </Form>
            </React.Fragment>
        );
    }
}