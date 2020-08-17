import React from 'react';
import axios from 'axios';
import * as auth from '../../services/authService';
import { Button, Form, FormGroup, Alert, Container } from 'reactstrap';
import {Input, CustomLink} from '../common/index';


export default class PasswordResetForm extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            email: '',
            successMessage: undefined,
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
            successMessage: undefined,
            [name]: target.value,
            errors: { ...this.state.errors, [name]: undefined },
        });
    };

    handleSubmit = async event => {
        event.preventDefault();
        await this.cancelPreviousRequests();
        const { email } = this.state;
        try {
            const response = await auth.resetPassword({email}, this.reqSource.token);
            // console.log(response);
            this.setState({
                successMessage: "Instructions for password changing are sent to your email.",
                errors: {},
                email: ''
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
        const { successMessage, email, errors } = this.state;

        return (
            <React.Fragment>
                <Container className="col-6 d-flex justify-content-center mx-auto">
                    <h2>Forgotten password</h2>
                </Container>
                <Form className="form-center pt-4" autoComplete="on">
                    <Input
                        name="email"
                        labelText="Email of your account"
                        value={email || ""}
                        error={errors.email}
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
                    <FormGroup className="mt-0 d-flex">
                        <Button
                            onClick={this.handleSubmit}
                            className="d-flex align-items-center mr-2"
                            size="md"
                        >
                            Submit
                        </Button>
                        <CustomLink
                            to="/login"
                            key="to-login"
                            tag={({onClick, children}) => (
                                <Button
                                    size="md"
                                    className="bg-dark d-flex align-items-center"
                                    onClick={onClick}
                                    children={children}
                                />
                            )}
                        >
                            Back to login
                        </CustomLink>
                    </FormGroup>
                </Form>
            </React.Fragment>
        );
    }
}