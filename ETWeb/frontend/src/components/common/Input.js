import { FormFeedback, FormGroup, Input as BaseInput, Label } from "reactstrap";
import React from "react";
import PropTypes from "prop-types";


export default class Input extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            name: props.name,
            labelText: props.labelText,
            labelFirst: props.labelFirst,
            type: props.type,
            required: props.required,
            placeholder: props.placeholder,
            accept: props.accept,
            autoComplete: props.autoComplete
        };
        Object.keys(props).filter(key => key.match(/on/)).forEach(eventKey => {
            this.state[eventKey] = this.props[eventKey];
        });
    }

    render() {
        const { name, labelText, labelFirst, type, required, placeholder, accept, ...events } = this.state;
        const label = <Label for={name}>{labelText}</Label>;
        return (
            <FormGroup>
                {labelFirst && label}
                <BaseInput
                    type={type}
                    value={this.props.value}
                    disabled={this.props.disabled}
                    invalid={this.props.error !== undefined}
                    // autoComplete={type === 'password' ? "new-password" : "none"}
                    name={name}
                    id={`${name}`}
                    placeholder={`${placeholder}${required ? "*" : ""}`}
                    accept={accept}
                    {...events}
                />
                {!labelFirst && label}
                {this.props.error && (<FormFeedback>{this.props.error}</FormFeedback>)}
            </FormGroup>
        );
    }
}

Input.propTypes = {
    name: PropTypes.string.isRequired,
    labelText: PropTypes.string,
    placeholder: PropTypes.string,
    type: PropTypes.string,
    autoComplete: PropTypes.string,
    required: PropTypes.bool,
    error: PropTypes.string
};

Input.defaultProps = {
    labelText: "",
    labelFirst: true,
    type: "text",
    placeholder: "",
    required: true,
    error: undefined,
};