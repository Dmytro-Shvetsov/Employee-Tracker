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
            value: props.value
        }
    }

    render() {
        const { name, labelText, type, required, placeholder, value } = this.state;
        return (
            <FormGroup>
                <Label for={name}>{labelText}</Label>
                <BaseInput
                    type={type}
                    value={value}
                    disabled={this.props.disabled !== undefined}
                    invalid={this.props.error !== undefined}
                    autoComplete={type === 'password' ? "new-password" : ""}
                    onChange={this.props.onInputChange}
                    name={name}
                    id={`#${name}`}
                    placeholder={`${placeholder}${required ? "*" : ""}`}
                />
                <FormFeedback>{this.props.error}</FormFeedback>
            </FormGroup>
        );
    }
}

Input.propTypes = {
    name: PropTypes.string.isRequired,
    onInputChange: PropTypes.func.isRequired,
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