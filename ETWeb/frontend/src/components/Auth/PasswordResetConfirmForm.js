import React from 'react';
import axios from 'axios';
import * as auth from '../../services/authService';
import { Button, Form, FormGroup, Label, Alert, Container } from 'reactstrap';
import { Redirect } from 'react-router-dom';
import {Input} from '../common/index'


export default class PasswordResetConfirmForm extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            data: {
                password: '',
                token: undefined
            },
            successMessage: undefined,
            errors: {},
            passwordResetFinished: false
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
            successMessage: undefined,
            data: { ...this.state.data, [name]: target.value },
            errors: { ...this.state.errors, [name]: undefined },
        });
    };

    validateToken = async token => {
        await this.cancelPreviousRequests();
        try {
            const response = await auth.validatePasswordResetToken({token}, this.reqSource.token);
            this.setState({data: {...this.state.data, token}})
        } catch (error) {
            console.log(error.message);
            this.setState({data: {...this.state.data, token: null}})
        }
    };

    handleSubmit = async event => {
        event.preventDefault();
        await this.cancelPreviousRequests();
        const { data } = this.state;

        if (data.password !== data.password2) {
            this.setState({errors: {...this.state.errors, password: "Passwords do not match"}});
            return;
        }

        try {
            const response = await auth.confirmPasswordReset(data, this.reqSource.token);
            console.log(response);
            this.setState({
                successMessage: "Your password was successfully changed.c" +
                    "You may now log in with a new one. Redirecting...",
                data: {token: ''},
                errors: {},
                loginFinished: true
            });
            this.timeout = setTimeout(() => this.setState({passwordResetFinished: true}), 3000);
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

    async componentDidMount() {
        this._isMounted = true;
        await this.validateToken(this.props.match.params.token);
    }

    async componentWillUnmount() {
        this._isMounted = false;
        await this.cancelPreviousRequests();
        if (this.timeout !== undefined) {
            clearTimeout(this.timeout);
        }
    }

    render() {
        const { successMessage, data, errors, passwordResetFinished } = this.state;

        if (passwordResetFinished) {
            return <Redirect to="/login"/>
        } else if (data.token === null) {
            console.log("invalid token");
            return <Redirect to="/"/>
        } else if (data.token === undefined) {
            return <span>Wrapping up...</span>;
        } else {
            return (
                <React.Fragment>
                    <Container className="col-6 d-flex justify-content-center mx-auto">
                        <h2>Create new password</h2>
                    </Container>
                    <Form className="form-center pt-4" autoComplete="on">
                        <Input
                            name="password"
                            labelText="Enter a new password"
                            type="password"
                            value={data.password || ""}
                            error={errors.password}
                            onChange={this.handleInputChange}
                        />
                        <Input
                            name="password2"
                            labelText="Confirm new password"
                            type="password"
                            value={data.password2 || ""}
                            error={errors.password2}
                            onChange={this.handleInputChange}
                        />
                        <FormGroup>
                            <Alert color="danger" isOpen={errors.non_field_errors !== undefined}>
                                {errors.non_field_errors}
                            </Alert>
                        </FormGroup>
                        <FormGroup>
                            <Alert color="success" isOpen={successMessage !== undefined}>
                                {successMessage}
                            </Alert>
                        </FormGroup>
                        <FormGroup className="auth-other">
                            <Button onClick={this.handleSubmit}>Submit</Button>
                        </FormGroup>
                    </Form>
                </React.Fragment>
            );
        }
    }
}
