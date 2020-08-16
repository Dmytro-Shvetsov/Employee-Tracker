import React from 'react';
import axios from 'axios';
import * as auth from '../../services/authService.js'
import { Button, Form, FormGroup, Alert } from 'reactstrap';
import { Container } from "reactstrap";
import { Link, Redirect } from 'react-router-dom';
import {Input} from '../common/index'


export default class RegisterForm extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            data: {
                username: '',
                email: '',
                password: '',
                password2: '',
                isStaff: false,
            },
            errors: {},
            registrationFinished: false
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
            data: { ...this.state.data, [name]: target.value},
            errors: {...this.state.errors, [name]: undefined},
        });
    };

    handleIsStaffChange = event => {
        const {data} = this.state;
        this.setState({
            data: {
                ...data,
                isStaff: !this.state.data.isStaff
            },
        });
    };

    handleSubmit = async event => {
        event.preventDefault();
        await this.cancelPreviousRequests();
        try {
            const response = await auth.registerUser(this.state.data, this.reqSource.token);
            alert(response.data.detail);
            this.setState({
                registrationFinished: true
            });
        } catch (error) {
            console.log(error.message);
            if (error.response.status === 400) {
                const fieldErrors = error.response.data;
                Object.keys(fieldErrors).map((fieldName) => {
                    fieldErrors[fieldName] = fieldErrors[fieldName].join(" ");
                    console.log(fieldErrors[fieldName]);
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
        const { errors, registrationFinished } = this.state;

        if (registrationFinished) {
            return <Redirect to="/login"/>
        }
        return (
            <React.Fragment>
                <Container className="col-8" style={{margin: "auto"}}>
                    <h1>Account Sign Up</h1>
                    <p>
                        Use the form below to sign in to your online account.
                        Please enter your information exactly as given to you.
                        If you are having trouble please use the 'forgot username/password'
                        link or <Link to = "/contact">contact our staff</Link> for assistance.
                    </p>
                </Container>
                <Form className="form-center" autoComplete="on">
                    <Input
                        name="username"
                        labelText="Username"
                        error={errors.username}
                        onChange={this.handleInputChange}
                    />
                    <Input
                        name="email"
                        labelText="Email"
                        error={errors.email}
                        onChange={this.handleInputChange}
                    />
                    <Input
                        name="password"
                        labelText="Password"
                        error={errors.password}
                        onChange={this.handleInputChange}
                        type="password"
                    />
                    <Input
                        name="password2"
                        labelText="Confirm password"
                        error={errors.password2}
                        onChange={this.handleInputChange}
                        type="password"
                    />
                    <Input
                        name="isStaff"
                        labelText="I'd like to create a manager account"
                        labelFirst={false}
                        type="checkbox"
                        onChange={this.handleIsStaffChange}
                    />

                    {/*<FormGroup check>*/}
                    {/*    <Label check>*/}
                    {/*        */}
                    {/*    </Label>*/}
                    {/*</FormGroup>*/}
                    <FormGroup>
                        <Alert color="danger" isOpen={errors.non_field_errors !== undefined}>
                            {errors.non_field_errors}
                        </Alert>
                    </FormGroup>
                    <FormGroup className="auth-other">
                        <Button onClick={(event) => this.handleSubmit(event)}>Submit</Button>
                    </FormGroup>
                </Form>
            </React.Fragment>
        );
    }
}