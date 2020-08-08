import { FormFeedback, FormGroup, Input as BaseInput, Label } from "reactstrap";
import React from "react";
import PropTypes from "prop-types";


export default class Input extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            name: props.name,
            labelText: props.labelText,
            type: props.type,
            required: props.required,
            placeholder: props.placeholder,
            accept: props.accept,
        };
        Object.keys(props).filter(key => key.match(/on/)).forEach(eventKey => {
            this.state[eventKey] = this.props[eventKey];
        });
    }

    render() {
        const { name, labelText, type, required, placeholder, accept, ...events } = this.state;
        return (
            <FormGroup>
                <Label for={name}>{labelText}</Label>
                <BaseInput
                    type={type}
                    value={this.props.value}
                    disabled={this.props.disabled}
                    invalid={this.props.error !== undefined}
                    autoComplete={type === 'password' ? "new-password" : ""}
                    name={name}
                    id={`${name}`}
                    placeholder={`${placeholder}${required ? "*" : ""}`}
                    accept={accept}
                    {...events}
                />
                <FormFeedback>{this.props.error}</FormFeedback>
            </FormGroup>
        );
    }
}

Input.propTypes = {
    name: PropTypes.string.isRequired,
    // onChange: PropTypes.func.isRequired,
    labelText: PropTypes.string,
    placeholder: PropTypes.string,
    type: PropTypes.string,
    required: PropTypes.bool,
    error: PropTypes.string
};

Input.defaultProps = {
    labelText: "",
    type: "text",
    placeholder: "",
    required: true,
    error: undefined,
};